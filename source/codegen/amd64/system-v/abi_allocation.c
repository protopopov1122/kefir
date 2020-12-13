#include "kefir/codegen/amd64/system-v/abi_allocation.h"
#include "kefir/core/error.h"
#include "kefir/core/mem.h"

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
    kefir_size_t slot;
};

static kefir_result_t assign_immediate_integer(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, info->slot++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_INTEGER;
    allocation->index = index;
    allocation->requirements.integer = 1;
    return KEFIR_OK;
}

static kefir_result_t assign_immediate_sse(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, info->slot++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_SSE;
    allocation->index = index;
    allocation->requirements.sse = 1;
    return KEFIR_OK;
}

static kefir_result_t assign_immediate_memory(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, info->slot++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_IMMEDIATE;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    allocation->index = index;
    allocation->requirements.memory.size = layout->size;
    allocation->requirements.memory.alignment = layout->alignment;
    return KEFIR_OK;
}

static kefir_result_t assign_immediate_pad(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, info->slot++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_SKIP;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    allocation->index = index;
    return KEFIR_OK;
}

struct recursive_aggregate_allocation {
    const struct kefir_ir_type *type;
    const struct kefir_vector *layout;
    struct kefir_vector *allocation;
    struct kefir_amd64_sysv_input_parameter_allocation *top_allocation;
    struct kefir_ir_type_visitor *visitor;
    kefir_size_t *slot;
    kefir_size_t nested;
    struct kefir_amd64_sysv_abi_qword_position init_position;
    struct kefir_amd64_sysv_abi_qword_position max_position;
};

static kefir_result_t assign_nested_scalar(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    struct recursive_aggregate_allocation *info = 
        (struct recursive_aggregate_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, (*info->slot)++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_NESTED;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    allocation->index = index;
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

static kefir_result_t assign_nested_memory(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct recursive_aggregate_allocation *info = 
        (struct recursive_aggregate_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, (*info->slot)++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_NESTED;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    allocation->index = index;
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_next(&info->top_allocation->container,
                                                KEFIR_AMD64_SYSV_PARAM_MEMORY,
                                                layout->size,
                                                layout->alignment,
                                                &allocation->container_reference));
    return KEFIR_OK;
}

static kefir_result_t assign_nested_struct(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(typeentry);
    struct recursive_aggregate_allocation *info = 
        (struct recursive_aggregate_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, (*info->slot)++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    allocation->index = index;
    info->nested++;
    REQUIRE_OK(kefir_ir_type_visitor_list_subtrees(type, info->visitor, (void *) info, index + 1, typeentry->param));
    info->nested--;
    return KEFIR_OK;
}

static kefir_result_t nested_visitor_init(struct kefir_ir_type_visitor *);

static kefir_result_t union_reset_hook(const struct kefir_ir_type *type,
                                     kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry,
                                     void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    struct recursive_aggregate_allocation *info = 
        (struct recursive_aggregate_allocation *) payload;
    if (info->nested == 0) {
        struct kefir_amd64_sysv_abi_qword_position current_position;
        REQUIRE_OK(kefir_amd64_sysv_abi_qwords_save_position(&info->top_allocation->container, &current_position));
        REQUIRE_OK(kefir_amd64_sysv_abi_qwords_max_position(&info->max_position,
                                                        &current_position,
                                                        &info->max_position));
        return kefir_amd64_sysv_abi_qwords_restore_position(&info->top_allocation->container, &info->init_position);
    } else {
        return KEFIR_OK;
    }
}

static kefir_result_t assign_nested_union(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(typeentry);
    struct recursive_aggregate_allocation *info = 
        (struct recursive_aggregate_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, (*info->slot)++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    allocation->index = index;
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(nested_visitor_init(&visitor));
    visitor.posthook = union_reset_hook;
    struct recursive_aggregate_allocation nested_info = {
        .type = type,
        .layout = info->layout,
        .allocation = info->allocation,
        .top_allocation = info->top_allocation,
        .visitor = &visitor,
        .slot = info->slot,
        .nested = 0
    };
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_save_position(&info->top_allocation->container, &nested_info.init_position));
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_save_position(&info->top_allocation->container, &nested_info.max_position));
    REQUIRE_OK(kefir_ir_type_visitor_list_subtrees(type, &visitor, (void *) &nested_info, index + 1, typeentry->param));
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_restore_position(&info->top_allocation->container, &nested_info.max_position));
    return KEFIR_OK;
}

static kefir_result_t assign_nested_array(const struct kefir_ir_type *type,
                                        kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry,
                                        void *payload) {
    UNUSED(typeentry);
    struct recursive_aggregate_allocation *info = 
        (struct recursive_aggregate_allocation *) payload;
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, (*info->slot)++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    allocation->index = index;
    info->nested++;
    for (kefir_size_t i = 0; i < (kefir_size_t) typeentry->param; i++) {
        REQUIRE_OK(kefir_ir_type_visitor_list_subtrees(type, info->visitor, (void *) info, index + 1, 1));
    }
    info->nested--;
    return KEFIR_OK;
}

static kefir_result_t struct_disown(struct kefir_mem *mem,
                                  struct kefir_amd64_sysv_input_parameter_allocation *alloc) {
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_free(&alloc->container, mem));
    alloc->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    alloc->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    return KEFIR_OK;
}

static kefir_result_t aggregate_postmerger(struct kefir_mem *mem,
                                         struct kefir_amd64_sysv_input_parameter_allocation *alloc) {
    const kefir_size_t length = kefir_vector_length(&alloc->container.qwords);
    kefir_size_t x87count = 0;
    kefir_size_t ssecount = 0;
    bool had_nonsseup = false;
    for (kefir_size_t i = 0; i < length; i++) {
        struct kefir_amd64_sysv_abi_qword *qword =
            (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&alloc->container.qwords, i);
        had_nonsseup = had_nonsseup || (i > 0 && qword->klass != KEFIR_AMD64_SYSV_PARAM_SSEUP);
        if (qword->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
            return struct_disown(mem, alloc);
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_X87) {
            x87count++;
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_X87UP) {
            if (x87count == 0) {
                return struct_disown(mem, alloc);
            } else {
                x87count--;
            }
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_SSE) {
            ssecount++;
        } else if (qword->klass == KEFIR_AMD64_SYSV_PARAM_SSEUP) {
            if (ssecount == 0) {
                qword->klass = KEFIR_AMD64_SYSV_PARAM_SSE;
            } else {
                ssecount--;
            }
        }
    }
    if (length > 2 &&
        (((struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&alloc->container.qwords, 0))->klass != KEFIR_AMD64_SYSV_PARAM_SSE ||
        had_nonsseup)) {
        return struct_disown(mem, alloc);
    }
    return KEFIR_OK;
}

static kefir_result_t nested_visitor_init(struct kefir_ir_type_visitor *visitor) {
    kefir_ir_type_visitor_init(visitor, visitor_not_supported);
    KEFIR_IR_TYPE_VISITOR_INIT_SCALARS(visitor, assign_nested_scalar);
    visitor->visit[KEFIR_IR_TYPE_MEMORY] = assign_nested_memory;
    visitor->visit[KEFIR_IR_TYPE_PAD] = assign_nested_memory;
    visitor->visit[KEFIR_IR_TYPE_STRUCT] = assign_nested_struct;
    visitor->visit[KEFIR_IR_TYPE_ARRAY] = assign_nested_array;
    visitor->visit[KEFIR_IR_TYPE_UNION] = assign_nested_union;
    return KEFIR_OK;
}

static kefir_result_t immediate_struct_unwrap(struct kefir_mem *mem,
                                            const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            const struct kefir_amd64_sysv_data_layout *top_layout,
                                            const struct kefir_vector *layout,
                                            struct kefir_vector *allocation,
                                            kefir_size_t *slot,
                                            struct kefir_amd64_sysv_input_parameter_allocation *top_allocation) {
    kefir_size_t qword_count = top_layout->size / KEFIR_AMD64_SYSV_ABI_QWORD;
    if (top_layout->size % KEFIR_AMD64_SYSV_ABI_QWORD != 0) {
        qword_count++;
    }
    top_allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER;
    top_allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(nested_visitor_init(&visitor));
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_alloc(&top_allocation->container, mem, qword_count));
    struct recursive_aggregate_allocation info = {
        .type = type,
        .layout = layout,
        .allocation = allocation,
        .top_allocation = top_allocation,
        .visitor = &visitor,
        .slot = slot,
        .nested = 0
    };
    kefir_result_t res = kefir_ir_type_visitor_list_subtrees(type, &visitor, (void *) &info, index + 1, typeentry->param);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_abi_qwords_free(&top_allocation->container, mem);
        return res;
    });
    return aggregate_postmerger(mem, top_allocation);
}

static kefir_result_t calculate_qword_requirements(struct kefir_amd64_sysv_input_parameter_allocation *allocation,
                                                 struct kefir_amd64_sysv_data_layout *layout) {
    allocation->requirements.integer = 0;
    allocation->requirements.sse = 0;
    allocation->requirements.sseup = 0;
    if (allocation->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        allocation->requirements.memory.size = layout->size;
        allocation->requirements.memory.alignment = layout->alignment;
    } else {
        allocation->requirements.memory.size = 0;
        allocation->requirements.memory.alignment = 0;
        for (kefir_size_t i = 0; i < kefir_vector_length(&allocation->container.qwords); i++) {
            struct kefir_amd64_sysv_abi_qword *qword =
                (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&allocation->container.qwords, i);
            switch (qword->klass) {
                case KEFIR_AMD64_SYSV_PARAM_INTEGER:
                    allocation->requirements.integer++;
                    break;

                case KEFIR_AMD64_SYSV_PARAM_SSE:
                    allocation->requirements.sse++;
                    break;

                case KEFIR_AMD64_SYSV_PARAM_SSEUP:
                    allocation->requirements.sseup++;
                    break;
                
                default:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected parameter class in QWord vector");
            }
        }
    }
    return KEFIR_OK;
}

static kefir_result_t assign_immediate_struct(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, info->slot++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    allocation->index = index;

    if (layout->size > 8 * KEFIR_AMD64_SYSV_ABI_QWORD || !layout->aligned) {
        allocation->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    } else {
        REQUIRE_OK(immediate_struct_unwrap(
            info->mem, type, index, typeentry, layout, info->layout, info->allocation, &info->slot, allocation));
    }
    return calculate_qword_requirements(allocation, layout);
}

static kefir_result_t immediate_array_unwrap(struct kefir_mem *mem,
                                            const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            const struct kefir_amd64_sysv_data_layout *top_layout,
                                            const struct kefir_vector *layout,
                                            struct kefir_vector *allocation,
                                            kefir_size_t *slot,
                                            struct kefir_amd64_sysv_input_parameter_allocation *top_allocation) {
    UNUSED(typeentry);
    kefir_size_t qword_count = top_layout->size / KEFIR_AMD64_SYSV_ABI_QWORD;
    if (top_layout->size % KEFIR_AMD64_SYSV_ABI_QWORD != 0) {
        qword_count++;
    }
    top_allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER;
    top_allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(nested_visitor_init(&visitor));
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_alloc(&top_allocation->container, mem, qword_count));
    struct recursive_aggregate_allocation info = {
        .type = type,
        .layout = layout,
        .allocation = allocation,
        .top_allocation = top_allocation,
        .visitor = &visitor,
        .slot = slot,
        .nested = 0
    };
    for (kefir_size_t i = 0; i < (kefir_size_t) typeentry->param; i++) {
        kefir_result_t res = kefir_ir_type_visitor_list_subtrees(type, &visitor, (void *) &info, index + 1, 1);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_amd64_sysv_abi_qwords_free(&top_allocation->container, mem);
            return res;
        });
    }
    return aggregate_postmerger(mem, top_allocation);
}

static kefir_result_t assign_immediate_array(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, info->slot++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    allocation->index = index;

    if (layout->size > 8 * KEFIR_AMD64_SYSV_ABI_QWORD || !layout->aligned) {
        allocation->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    } else {
        immediate_array_unwrap(info->mem, type, index, typeentry, layout, info->layout, info->allocation, &info->slot, allocation);
    }
    return calculate_qword_requirements(allocation, layout);
}

static kefir_result_t immediate_union_unwrap(struct kefir_mem *mem,
                                            const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            const struct kefir_amd64_sysv_data_layout *top_layout,
                                            const struct kefir_vector *layout,
                                            struct kefir_vector *allocation,
                                            kefir_size_t *slot,
                                            struct kefir_amd64_sysv_input_parameter_allocation *top_allocation) {
    UNUSED(typeentry);
    kefir_size_t qword_count = top_layout->size / KEFIR_AMD64_SYSV_ABI_QWORD;
    if (top_layout->size % KEFIR_AMD64_SYSV_ABI_QWORD != 0) {
        qword_count++;
    }
    top_allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER;
    top_allocation->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(nested_visitor_init(&visitor));
    visitor.posthook = union_reset_hook;
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_alloc(&top_allocation->container, mem, qword_count));
    struct recursive_aggregate_allocation info = {
        .type = type,
        .layout = layout,
        .allocation = allocation,
        .top_allocation = top_allocation,
        .visitor = &visitor,
        .slot = slot,
        .nested = 0
    };
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_save_position(&top_allocation->container, &info.init_position));
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_save_position(&top_allocation->container, &info.max_position));
    kefir_result_t res = kefir_ir_type_visitor_list_subtrees(type, &visitor, (void *) &info, index + 1, typeentry->param);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_abi_qwords_free(&top_allocation->container, mem);
        return res;
    });
    REQUIRE_OK(kefir_amd64_sysv_abi_qwords_restore_position(&top_allocation->container, &info.max_position));
    return aggregate_postmerger(mem, top_allocation);
}

static kefir_result_t assign_immediate_union(const struct kefir_ir_type *type,
                                    kefir_size_t index,
                                    const struct kefir_ir_typeentry *typeentry,
                                    void *payload) {
    struct input_allocation *info =
        (struct input_allocation *) payload;
    struct kefir_amd64_sysv_data_layout *layout =
        (struct kefir_amd64_sysv_data_layout *) kefir_vector_at(info->layout, index);
    struct kefir_amd64_sysv_input_parameter_allocation *allocation =
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(info->allocation, info->slot++);
    allocation->type = KEFIR_AMD64_SYSV_INPUT_PARAM_CONTAINER;
    allocation->index = index;

    if (layout->size > 8 * KEFIR_AMD64_SYSV_ABI_QWORD || !layout->aligned) {
        allocation->klass = KEFIR_AMD64_SYSV_PARAM_MEMORY;
    } else {
        immediate_union_unwrap(info->mem, type, index, typeentry, layout, info->layout, info->allocation, &info->slot, allocation);
    }
    return calculate_qword_requirements(allocation, layout);
}

kefir_result_t kefir_amd64_sysv_input_parameter_classify(struct kefir_mem *mem,
                                                     const struct kefir_ir_type *type,
                                                     const struct kefir_vector *layout,
                                                     struct kefir_vector *allocation) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type definition"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid layout vector"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid allocation vector"));
    kefir_size_t slots = kefir_ir_type_total_slots(type);
    REQUIRE_OK(kefir_vector_alloc(mem, sizeof(struct kefir_amd64_sysv_input_parameter_allocation),
        slots, allocation));
    kefir_result_t res = kefir_vector_extend(allocation, slots);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, allocation);
        return res;
    });
    for (kefir_size_t i = 0; i < slots; i++) {
        struct kefir_amd64_sysv_input_parameter_allocation *entry =
            (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(allocation, i);
        entry->type = KEFIR_AMD64_SYSV_INPUT_PARAM_SKIP;
        entry->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
        entry->index = 0;
        entry->requirements.integer = 0;
        entry->requirements.sse = 0;
        entry->requirements.sseup = 0;
        entry->requirements.memory.size = 0;
        entry->requirements.memory.alignment = 0;
        entry->location.integer = KEFIR_AMD64_SYSV_INPUT_PARAMETER_NONE;
        entry->location.sse = KEFIR_AMD64_SYSV_INPUT_PARAMETER_NONE;
        entry->location.sseup = KEFIR_AMD64_SYSV_INPUT_PARAMETER_NONE;
        entry->location.memory = KEFIR_AMD64_SYSV_INPUT_PARAMETER_NONE;
    }
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, visitor_not_supported);
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, assign_immediate_integer);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, assign_immediate_sse);
    visitor.visit[KEFIR_IR_TYPE_MEMORY] = assign_immediate_memory;
    visitor.visit[KEFIR_IR_TYPE_PAD] = assign_immediate_pad;
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = assign_immediate_struct;
    visitor.visit[KEFIR_IR_TYPE_UNION] = assign_immediate_union;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = assign_immediate_array;
    struct input_allocation info = {
        .mem = mem,
        .layout = layout,
        .allocation = allocation
    };
    res = kefir_ir_type_visitor_list_subtrees(type, &visitor, (void *) &info, 0, kefir_ir_type_length(type));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, allocation);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_input_parameter_free(struct kefir_mem *mem,
                                                 struct kefir_vector *allocation) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(allocation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid allocation vector"));
    for (kefir_size_t i = 0; i < kefir_vector_length(allocation); i++) {
        struct kefir_amd64_sysv_input_parameter_allocation *alloc =
            (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(allocation, i);
        if (alloc->type == KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER) {
            REQUIRE_OK(kefir_amd64_sysv_abi_qwords_free(&alloc->container, mem));
        }
    }
    return kefir_vector_free(mem, allocation);
}

struct allocation_state {
    struct kefir_amd64_sysv_input_parameter_location current;
    const struct kefir_vector *layout;
    struct kefir_vector *allocation;
};

static kefir_result_t integer_allocate(const struct kefir_ir_type *type,
                                     kefir_size_t index,
                                     const struct kefir_ir_typeentry *typeentry,
                                     void *payload) {
    UNUSED(index);
    UNUSED(typeentry);
    struct allocation_state *state = (struct allocation_state *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    struct kefir_amd64_sysv_input_parameter_allocation *alloc = 
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(state->allocation, iter.slot);
    REQUIRE(alloc->requirements.integer == 1 &&
            alloc->requirements.sse == 0 &&
            alloc->requirements.sseup == 0 &&
            alloc->requirements.memory.size == 0,
        KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected INTEGER to require exactly 1 int eightbyte"));
    alloc->location.integer = 0;
    alloc->location.sse = 0;
    alloc->location.sseup = 0;
    alloc->location.memory = 0;
    if (state->current.integer + 1 <= KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT) {
        alloc->location.integer = state->current.integer++;
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Allocating INTEGERs on stack is not supported yet");
    }
    return KEFIR_OK;
}

static kefir_result_t sse_allocate(const struct kefir_ir_type *type,
                                 kefir_size_t index,
                                 const struct kefir_ir_typeentry *typeentry,
                                 void *payload) {
    UNUSED(index);
    UNUSED(typeentry);
    struct allocation_state *state = (struct allocation_state *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    struct kefir_amd64_sysv_input_parameter_allocation *alloc = 
        (struct kefir_amd64_sysv_input_parameter_allocation *) kefir_vector_at(state->allocation, iter.slot);
    REQUIRE(alloc->requirements.integer == 0 &&
            alloc->requirements.sse == 1 &&
            alloc->requirements.sseup == 0 &&
            alloc->requirements.memory.size == 0,
        KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected SSE to require exactly 1 sse eightbyte"));
    alloc->location.integer = 0;
    alloc->location.sse = 0;
    alloc->location.sseup = 0;
    alloc->location.memory = 0;
    if (state->current.sse + 1 <= KEFIR_AMD64_SYSV_SSE_REGISTER_COUNT) {
        alloc->location.sse = state->current.sse++;
        state->current.sseup = state->current.sseup;
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Allocating SSEs on stack is not supported yet");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_input_parameter_allocate(const struct kefir_ir_type *type,
                                                     const struct kefir_vector *layout,
                                                     struct kefir_vector *allocation) {
    UNUSED(layout);
    UNUSED(allocation);

    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, visitor_not_supported);
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, integer_allocate);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, sse_allocate);
    struct allocation_state state = {
        .current = {
            .integer = 0,
            .sse = 0,
            .sseup = 0,
            .memory = 0
        },
        .layout = layout,
        .allocation = allocation
    };
    REQUIRE_OK(kefir_ir_type_visitor_list_subtrees(type, &visitor, (void *) &state, 0, kefir_ir_type_length(type)));
    return KEFIR_OK;
}