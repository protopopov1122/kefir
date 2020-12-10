#include "kefir/codegen/amd64/system-v/abi_allocation.h"
#include "kefir/core/error.h"

#define QWORD 8

struct qword_counter {
    const struct kefir_vector *layout;
    kefir_size_t count;
};

static kefir_result_t count_qwords_visitor(const struct kefir_ir_type *type,
                                  kefir_size_t index,
                                  const struct kefir_ir_typeentry *typeentry,
                                  void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct qword_counter *counter = (struct qword_counter *) payload;
    const struct kefir_amd64_sysv_data_layout *layout =
        (const struct kefir_amd64_sysv_data_layout *) kefir_vector_at(counter->layout, index);
    kefir_size_t count = layout->size / QWORD;
    if (layout->size % QWORD != 0) {
        count++;
    }
    counter->count += count;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_count(const struct kefir_ir_type *type,
                                             const struct kefir_vector *layout,
                                             kefir_size_t *count) {
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, count_qwords_visitor);
    struct qword_counter counter = {
        .layout = layout,
        .count = 0
    };
    REQUIRE_OK(kefir_ir_type_visitor_traverse_subtrees(type, &visitor, (void *) &counter, 0, kefir_ir_type_length(type)));
    *count = counter.count;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_alloc(struct kefir_amd64_sysv_abi_qwords *qwords,
                                             struct kefir_mem *mem,
                                             kefir_size_t qword_count) {
    REQUIRE(qwords != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(qword_count > 0, KEFIR_MALFORMED_ARG);
    REQUIRE_OK(kefir_vector_alloc(mem, sizeof(struct kefir_amd64_sysv_abi_qword), qword_count, &qwords->qwords));
    kefir_vector_extend(&qwords->qwords, qword_count);
    for (kefir_size_t i = 0; i < qword_count; i++) {
        struct kefir_amd64_sysv_abi_qword *qword =
            (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&qwords->qwords, i);
        qword->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
        qword->index = i;
        qword->current_offset = 0;
    }
    qwords->current = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_free(struct kefir_amd64_sysv_abi_qwords *qwords,
                                            struct kefir_mem *mem) {
    REQUIRE(qwords != NULL, KEFIR_MALFORMED_ARG);
    return kefir_vector_free(mem, &qwords->qwords);
}

static kefir_amd64_sysv_data_class_t derive_dataclass(kefir_amd64_sysv_data_class_t first,
                                                    kefir_amd64_sysv_data_class_t second) {
    if (first == second) {
        return first;
    }
    if (first == KEFIR_AMD64_SYSV_PARAM_NO_CLASS) {
        return second;
    }
    if  (second == KEFIR_AMD64_SYSV_PARAM_NO_CLASS) {
        return first;
    }
#define ANY_OF(x, y, a) ((x) == (a) || (y) == (a))
    if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_MEMORY)) {
        return KEFIR_AMD64_SYSV_PARAM_MEMORY;
    }
    if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_INTEGER)) {
        return KEFIR_AMD64_SYSV_PARAM_INTEGER;
    }
    if (ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_X87) ||
        ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_X87UP) ||
        ANY_OF(first, second, KEFIR_AMD64_SYSV_PARAM_COMPLEX_X87)) {
        return KEFIR_AMD64_SYSV_PARAM_MEMORY;
    }
#undef ANY_OF
    return KEFIR_AMD64_SYSV_PARAM_SSE;
}

struct kefir_amd64_sysv_abi_qword *next_qword(struct kefir_amd64_sysv_abi_qwords *qwords,
                                            kefir_size_t alignment) {
    struct kefir_amd64_sysv_abi_qword *qword =
        (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&qwords->qwords, qwords->current);
    kefir_size_t unalign = qword->current_offset % alignment;
    kefir_size_t pad = unalign > 0
        ? alignment - unalign
        : 0;
    if (alignment == 0 ||
        qword->current_offset + pad >= QWORD) {
        qwords->current++;
        qword = (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&qwords->qwords, qwords->current);
    } else {
        qword->current_offset += pad;
    }
    return qword;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_next(struct kefir_amd64_sysv_abi_qwords *qwords,
                                            kefir_amd64_sysv_data_class_t dataclass,
                                            kefir_size_t size,
                                            kefir_size_t alignment,
                                            struct kefir_amd64_sysv_abi_qword_ref *ref) {
    REQUIRE(qwords != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(size > 0, KEFIR_MALFORMED_ARG);
    REQUIRE(ref != NULL, KEFIR_MALFORMED_ARG);
    struct kefir_amd64_sysv_abi_qword *first = NULL;
    while (size > 0) {
        struct kefir_amd64_sysv_abi_qword *current = next_qword(qwords, alignment);
        if (first == NULL) {
            first = current;
            ref->qword = current;
            ref->offset = current->current_offset;
        }
        kefir_size_t available = QWORD - current->current_offset;
        kefir_size_t chunk = MIN(available, size);
        current->current_offset += chunk;
        size -= chunk;
        current->klass = derive_dataclass(current->klass, dataclass);
        available = 1;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_abi_qwords_reset_class(struct kefir_amd64_sysv_abi_qwords *qwords,
                                                    kefir_amd64_sysv_data_class_t dataclass,
                                                    kefir_size_t begin,
                                                    kefir_size_t count) {
    REQUIRE(qwords != NULL, KEFIR_MALFORMED_ARG);
    const kefir_size_t length = kefir_vector_length(&qwords->qwords);
    REQUIRE(begin < length, KEFIR_OUT_OF_BOUNDS);
    REQUIRE(count > 0, KEFIR_MALFORMED_ARG);
    for (kefir_size_t i = begin; i < MIN(length, begin + count); i++) {
        struct kefir_amd64_sysv_abi_qword *qword =
            (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&qwords->qwords, i);
        qword->klass = dataclass;
    }
    return KEFIR_OK;
}

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Encountered not supported type code while traversing type");
}

struct input_allocation {
    struct kefir_mem *mem;
    const struct kefir_vector *layout;
    struct kefir_vector *allocation;
};

static kefir_result_t assign_integer(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_DIRECT;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_INTEGER;
    return KEFIR_OK;
}

static kefir_result_t assign_sse(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_DIRECT;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_SSE;
    return KEFIR_OK;
}

static kefir_result_t assign_memory(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_DIRECT;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    return KEFIR_OK;
}

static kefir_result_t assign_no_class(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_SKIP;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    return KEFIR_OK;
}

struct recursive_struct_allocation {
    const struct kefir_ir_type *type;
    const struct kefir_vector *layout;
    struct kefir_vector *allocation;
    struct kefir_amd64_sysv_input_parameter_allocation *top_allocation;
    struct kefir_ir_type_visitor *visitor;
};

static kefir_result_t recursive_assign_scalar(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    struct recursive_struct_allocation *info = 
        (struct recursive_struct_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_NESTED_DIRECT;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    kefir_amd64_sysv_data_class_t dataclass =
        (typeentry->typecode == KEFIR_IR_TYPE_FLOAT32 || typeentry->typecode == KEFIR_IR_TYPE_FLOAT32)
            ? KEFIR_AMD64_SYSV_PARAM_SSE
            : KEFIR_AMD64_SYSV_PARAM_INTEGER;
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_next(&info->top_allocation->container,
                                                dataclass,
                                                layout->size,
                                                layout->alignment,
                                                &allocation->container_reference));
    return KEFIR_OK;
}

static kefir_result_t recursive_assign_memory(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct recursive_struct_allocation *info = 
        (struct recursive_struct_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_NESTED_DIRECT;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_next(&info->top_allocation->container,
                                                KEFIR_AMD64_SYSV_PARAM_MEMORY,
                                                layout->size,
                                                layout->alignment,
                                                &allocation->container_reference));
    return KEFIR_OK;
}

static kefir_result_t recursive_assign_struct(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(typeentry);
    struct recursive_struct_allocation *info = 
        (struct recursive_struct_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    return kefir_ir_type_visitor_traverse_subtrees(type, info->visitor, (void *) info, index + 1, typeentry->param);
}

static kefir_result_t struct_postmerger(struct kefir_amd64_sysv_input_parameter_allocation *alloc) {
    const kefir_size_t length = kefir_vector_length(&alloc->container.qwords);
    kefir_size_t x87count = 0;
    kefir_size_t ssecount = 0;
    bool had_sseup = false;
    for (kefir_size_t i = 0; i < length; i++) {
        struct kefir_amd64_sysv_abi_qword *qword =
            (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&alloc->container.qwords, i);
        if (qword->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
            return kefir_amd64_sysv_abi_qwords_reset_class(&alloc->container,
                KEFIR_AMD64_SYSV_PARAM_MEMORY, 0, length);
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_X87) {
            x87count++;
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_X87UP) {
            if (x87count == 0) {
                return kefir_amd64_sysv_abi_qwords_reset_class(&alloc->container,
                    KEFIR_AMD64_SYSV_PARAM_MEMORY, 0, length);
            } else {
                x87count--;
            }
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_SSE) {
            ssecount++;
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_SSEUP) {
            had_sseup = true;
            if (ssecount == 0) {
                qword->klass = KEFIR_AMD64_SYSV_PARAM_SSE;
            } else {
                ssecount--;
            }
        }
    }
    if (length > 2 &&
        ((struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&alloc->container.qwords, 0))->klass != KEFIR_AMD64_SYSV_PARAM_SSE &&
        !had_sseup) {
        return kefir_amd64_sysv_abi_qwords_reset_class(&alloc->container,
            KEFIR_AMD64_SYSV_PARAM_MEMORY, 0, length);
    }
    return KEFIR_OK;
}

static kefir_result_t assign_struct_recursive(struct kefir_mem *mem,
                                            const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            const struct kefir_amd64_sysv_data_layout *top_layout,
                                            const struct kefir_vector *layout,
                                            struct kefir_vector *allocation,
                                            struct kefir_amd64_sysv_input_parameter_allocation *top_allocation) {
    kefir_size_t qword_count = top_layout->size / QWORD;
    if (top_layout->size % QWORD != 0) {
        qword_count++;
    }
    top_allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER;
    top_allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_alloc(&top_allocation->container, mem, qword_count));
    struct kefir_ir_type_visitor visitor;
    struct recursive_struct_allocation info = {
        .type = type,
        .layout = layout,
        .allocation = allocation,
        .top_allocation = top_allocation,
        .visitor = &visitor
    };
    kefir_ir_type_visitor_init(&visitor, visitor_not_supported);
    visitor.visit[KEFIR_IR_TYPE_INT8] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_INT16] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_INT32] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_INT64] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_CHAR] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_SHORT] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_INT] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_LONG] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_WORD] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_FLOAT32] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_FLOAT64] = recursive_assign_scalar;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = recursive_assign_memory;
    visitor.visit[KEFIR_IR_TYPE_PAD] = recursive_assign_memory;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = recursive_assign_struct;
    kefir_result_t res = kefir_ir_type_visitor_traverse_subtrees(type, &visitor, (void *) &info, index + 1, typeentry->param);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_abi_qwords_free(&top_allocation->container, mem);
        return res;
    });
    return struct_postmerger(top_allocation);
}

static kefir_result_t assign_struct(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, index);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;

    if (layout->size > 8 * QWORD || !layout->aligned) {
        allocation->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    } else {
        assign_struct_recursive(info->mem, type, index, typeentry, layout, info->layout, info->allocation, allocation);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_input_parameter_allocate(struct kefir_mem *mem,
                                                     const struct kefir_ir_type *type,
                                                     const struct kefir_vector *layout,
                                                     struct kefir_vector *allocation) {
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(layout != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(allocation != NULL, KEFIR_MALFORMED_ARG);
    kefir_size_t length = kefir_ir_type_length(type);
    REQUIRE_OK(kefir_vector_alloc(mem, sizeof(struct kefir_amd64_sysv_input_parameter_allocation),
        length, allocation));
    kefir_result_t res = kefir_vector_extend(allocation, length);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, allocation);
        return res;
    });
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, visitor_not_supported);
    visitor.visit[KEFIR_IR_TYPE_INT8] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_INT16] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_INT32] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_INT64] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_CHAR] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_SHORT] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_INT] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_LONG] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_WORD] = assign_integer;
    visitor.visit[KEFIR_IR_TYPE_FLOAT32] = assign_sse;
    visitor.visit[KEFIR_IR_TYPE_FLOAT64] = assign_sse;
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = assign_memory;
    visitor.visit[KEFIR_IR_TYPE_PAD] = assign_no_class;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = assign_struct;
    struct input_allocation info = {
        .mem = mem,
        .layout = layout,
        .allocation = allocation
    };
    res = kefir_ir_type_visitor_traverse_subtrees(type, &visitor, (void *) &info, 0, length);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, allocation);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_input_parameter_free(struct kefir_mem *mem,
                                                 struct kefir_vector *allocation) {
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(allocation != NULL, KEFIR_MALFORMED_ARG);
    for (kefir_size_t i = 0; i < kefir_vector_length(allocation); i++) {
        struct kefir_amd64_sysv_input_parameter_allocation *alloc =
            (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(allocation, i);
        if (alloc->type == KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER) {
            REQUIRE_OK(kefir_amd64_sysv_abi_qwords_free(&alloc->container, mem));
        }
    }
    return kefir_vector_free(mem, allocation);
}