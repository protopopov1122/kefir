#include <string.h>
#include "kefir/core/vector_util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

const void *kefir_vector_at_impl(kefir_size_t elt_size, const void *vector, kefir_size_t length, kefir_size_t index) {
    REQUIRE(vector != NULL, NULL);
    REQUIRE(index < length, NULL);
    REQUIRE(elt_size > 0, NULL);
    const char *bytes = (const char *) vector;
    bytes += index * elt_size;
    return (const void *) bytes;
}

kefir_result_t kefir_vector_append_impl(kefir_size_t elt_size,
                                    void *vector,
                                    kefir_size_t *length,
                                    kefir_size_t capacity,
                                    const void *elt) {
    REQUIRE(elt_size > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero element size"));
    REQUIRE(vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero vector content pointer"));
    REQUIRE(length != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero length pointer"));
    REQUIRE(elt != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL element pointer"));
    REQUIRE(*length + 1 <= capacity, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Append operation exceeds vector capacity"));
    const char *bytes = (const char *) vector;
    bytes += elt_size * (*length)++;
    memcpy((void *) bytes, elt, elt_size);
    return KEFIR_OK;
}

kefir_result_t kefir_vector_copy_impl(kefir_size_t elt_size,
                                  void *dst_vector,
                                  kefir_size_t *dst_length,
                                  kefir_size_t dst_capacity,
                                  const void *src_vector,
                                  kefir_size_t src_length) {
    REQUIRE(elt_size > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero element size"));
    REQUIRE(dst_vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL destination vector"));
    REQUIRE(dst_length != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL destination vector length pointer"));
    REQUIRE(src_vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL source vector pointer"));
    REQUIRE(*dst_length + src_length <= dst_capacity, 
        KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Copied range of elements exceeds destination capacity"));
    char *bytes = (char *) dst_vector;
    bytes += elt_size * (*dst_length);
    memcpy((void *) bytes, src_vector, elt_size * src_length);
    *dst_length += src_length;
    return KEFIR_OK;
}