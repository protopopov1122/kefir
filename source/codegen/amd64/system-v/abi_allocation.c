#include "kefir/codegen/amd64/system-v/abi_allocation.h"

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

static kefir_result_t count_qwords(const struct kefir_ir_type *type,
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
                                             const struct kefir_ir_type *type,
                                             const struct kefir_vector *layout) {
    REQUIRE(qwords != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(mem != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(type != NULL, KEFIR_MALFORMED_ARG);
    REQUIRE(layout != NULL, KEFIR_MALFORMED_ARG);
    kefir_size_t qword_count;
    REQUIRE_OK(count_qwords(type, layout, &qword_count));
    REQUIRE_OK(kefir_vector_alloc(mem, sizeof(struct kefir_amd64_sysv_abi_qword), qword_count, &qwords->qwords));
    kefir_vector_extend(&qwords->qwords, qword_count);
    for (kefir_size_t i = 0; i < qword_count; i++) {
        struct kefir_amd64_sysv_abi_qword *qword =
            (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&qwords->qwords, i);
        qword->klass = KEFIR_AMD64_SYSV_PARAM_NO_CLASS;
        qword->offset = 0;
    }
    qwords->current = 0;
    qwords->type = type;
    qwords->layout = layout;
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
    kefir_size_t unalign = qword->offset % alignment;
    kefir_size_t pad = unalign > 0
        ? alignment - unalign
        : 0;
    if (alignment == 0 ||
        qword->offset + pad >= QWORD) {
        qwords->current++;
        qword = (struct kefir_amd64_sysv_abi_qword *) kefir_vector_at(&qwords->qwords, qwords->current);
    } else {
        qword->offset += pad;
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
            ref->index = qwords->current;
            ref->offset = current->offset;
        }
        kefir_size_t available = QWORD - current->offset;
        kefir_size_t chunk = MIN(available, size);
        current->offset += chunk;
        size -= chunk;
        current->klass = derive_dataclass(current->klass, dataclass);
        available = 1;
    }
    return KEFIR_OK;
}