#ifndef KEFIR_CORE_VECTOR_H_
#define KEFIR_CORE_VECTOR_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef struct kefir_vector {
    kefir_size_t element_size;
    void *content;
    kefir_size_t length;
    kefir_size_t capacity;
} kefir_vector_t;

kefir_result_t kefir_vector_init(struct kefir_vector *, kefir_size_t, void *, kefir_size_t);
kefir_size_t kefir_vector_length(const struct kefir_vector *);
kefir_size_t kefir_vector_available(const struct kefir_vector *);
void *kefir_vector_at(const struct kefir_vector *, kefir_size_t);
kefir_result_t kefir_vector_append(struct kefir_vector *, const void *);
kefir_result_t kefir_vector_copy(struct kefir_vector *, void *, kefir_size_t);
kefir_result_t kefir_vector_extend(struct kefir_vector *, kefir_size_t);
kefir_result_t kefir_vector_alloc(struct kefir_mem *, kefir_size_t, kefir_size_t, struct kefir_vector *);
kefir_result_t kefir_vector_realloc(struct kefir_mem *, kefir_size_t, struct kefir_vector *);
kefir_result_t kefir_vector_free(struct kefir_mem *, struct kefir_vector *);

#endif
