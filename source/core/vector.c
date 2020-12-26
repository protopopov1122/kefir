#include <string.h>
#include "kefir/core/vector.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_vector_init(struct kefir_vector *vector, kefir_size_t elem_size, void *area, kefir_size_t capacity) {
    REQUIRE(vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL vector pointer"));
    REQUIRE(elem_size > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero vector element size"));
    REQUIRE((area != NULL && capacity > 0) || (area == NULL && capacity == 0),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL content pointer for non-zero capacity vector"));
    vector->element_size = elem_size;
    vector->content = area;
    vector->length = 0;
    vector->capacity = capacity;
    return KEFIR_OK;
}

kefir_size_t kefir_vector_length(const struct kefir_vector *vector) {
    REQUIRE(vector != NULL, 0);
    return vector->length;
}

kefir_size_t kefir_vector_available(const struct kefir_vector *vector) {
    REQUIRE(vector != NULL, 0);
    return vector->capacity - vector->length;
}

void *kefir_vector_at(const struct kefir_vector *vector, kefir_size_t index) {
    REQUIRE(vector != NULL, NULL);
    REQUIRE(index < vector->length, NULL);
    char *bytes = (char *) vector->content;
    bytes += index * vector->element_size;
    return (void *) bytes;
}

kefir_result_t kefir_vector_append(struct kefir_vector *vector, const void *value) {
    REQUIRE(vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL vector pointer"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL value pointer"));
    REQUIRE(vector->length < vector->capacity, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Appending a value exceeds vector capacity"));
    char *bytes = (char *) vector->content;
    bytes += vector->length * vector->element_size;
    memcpy((void *) bytes, value, vector->element_size);
    vector->length++;
    return KEFIR_OK;
}

kefir_result_t kefir_vector_extend(struct kefir_vector *vector, kefir_size_t count) {
    REQUIRE(vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL vector pointer"));
    REQUIRE(vector->length + count <= vector->capacity,
        KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Vector extension exceeds vector capacity"));
    vector->length += count;
    return KEFIR_OK;
}

kefir_result_t kefir_vector_alloc(struct kefir_mem *mem,
                              kefir_size_t element_size,
                              kefir_size_t capacity,
                              struct kefir_vector *vector) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(element_size > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-zero element size"));
    REQUIRE(vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL vector pointer"));
    if (capacity > 0) {
        vector->content = KEFIR_MALLOC(mem, element_size * capacity);
        REQUIRE(vector->content != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory for vector content"));
    } else {
        vector->content = NULL;
    }
    vector->element_size = element_size;
    vector->length = 0;
    vector->capacity = capacity;
    return KEFIR_OK;
}

kefir_result_t kefir_vector_realloc(struct kefir_mem *mem,
                                kefir_size_t new_capacity,
                                struct kefir_vector *vector) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL vector pointer"));
    REQUIRE(vector->content != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid vector content pointer"));
    REQUIRE(vector->length <= new_capacity, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Expected new capacity to fit current vector length"));
    vector->content = KEFIR_REALLOC(mem, vector->content, vector->element_size * new_capacity);
    if (vector->content == NULL) {
        vector->length = 0;
        vector->capacity = 0;
        vector->element_size = 0;
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE,
            "Failed to reallocate vector");
    }
    vector->capacity = new_capacity;
    return KEFIR_OK;
}

kefir_result_t kefir_vector_free(struct kefir_mem *mem, struct kefir_vector *vector) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(vector != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL vector pointer"));
    if (vector->content != NULL) {
        KEFIR_FREE(mem, vector->content);
        vector->content = NULL;
        vector->length = 0;
        vector->capacity = 0;
        vector->element_size = 0;
    }
    return KEFIR_OK;
}