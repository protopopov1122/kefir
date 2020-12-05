#ifndef KEFIR_CORE_VECTOR_UTIL_H_
#define KEFIR_CORE_VECTOR_UTIL_H_

#include "kefir/core/basic-types.h"

const void *kefir_vector_at_impl(kefir_size_t, const void *, kefir_size_t, kefir_size_t);
kefir_result_t kefir_vector_append_impl(kefir_size_t, void *, kefir_size_t *, kefir_size_t, const void *);
kefir_result_t kefir_vector_copy_impl(kefir_size_t, void *, kefir_size_t *, kefir_size_t, const void *, kefir_size_t);

#define KEFIR_VECTOR_AT(elt_type, vector, length, index) \
    ((elt_type *) (kefir_vector_at_impl(sizeof(elt_type), (vector), (length), (index))))
#define KEFIR_VECTOR_APPEND(elt_type, vector, length_ptr, capacity, elt_ptr) \
    (kefir_vector_append_impl(sizeof(elt_type), (vector), (length_ptr), (capacity), (elt_ptr)))
#define KEFIR_VECTOR_COPY(elt_type, dst_vector, dst_length_ptr, dst_capacity, src_vector, src_length) \
    (kefir_vector_copy_impl(sizeof(elt_type), (dst_vector), (dst_length_ptr), (dst_capacity), (src_vector), (src_length)))

#define KEFIR_VECTOR_STRUCT(name, elt_type, elt_name) \
    struct name { \
        elt_type *elt_name; \
        kefir_size_t length; \
        kefir_size_t capacity; \
    }

#endif