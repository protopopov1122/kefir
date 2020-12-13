#include <stdlib.h>
#include "kefir/ir/instr.h"
#include "kefir/core/util.h"
#include "kefir/core/vector_util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_irblock_init(struct kefir_irblock *bcblock, void *area, kefir_size_t capacity) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block pointer"));
    REQUIRE((area != NULL && capacity != 0) || (area == NULL && capacity == 0),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL content pointer for non-zero capacity IR block"));
    
    bcblock->code = (struct kefir_irinstr *) area;
    bcblock->length = 0;
    bcblock->capacity = capacity;
    return KEFIR_OK;
}

kefir_size_t kefir_irblock_available(const struct kefir_irblock *bcblock) {
    REQUIRE(bcblock != NULL, 0);
    return bcblock->capacity - bcblock->length;
}

kefir_size_t kefir_irblock_length(const struct kefir_irblock *bcblock) {
    REQUIRE(bcblock != NULL, 0);
    return bcblock->length;
}

const struct kefir_irinstr *kefir_irblock_at(const struct kefir_irblock *bcblock, kefir_size_t idx) {
    REQUIRE(bcblock != NULL, NULL);
    return KEFIR_VECTOR_AT(struct kefir_irinstr, bcblock->code, bcblock->length, idx);
}

kefir_result_t kefir_irblock_append(struct kefir_irblock *bcblock, kefir_iropcode_t opcode, kefir_int64_t arg) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    struct kefir_irinstr instr = {
        .opcode = opcode,
        .arg = arg
    };
    return KEFIR_VECTOR_APPEND(struct kefir_irinstr, bcblock->code, &bcblock->length, bcblock->capacity, &instr);
}

kefir_result_t kefir_irblock_copy(struct kefir_irblock *dst, const struct kefir_irblock *src) {
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL destination IR block pointer"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL source IR block pointer"));
    return KEFIR_VECTOR_COPY(struct kefir_irinstr,
                           dst->code,
                           &dst->length,
                           dst->capacity,
                           src->code,
                           src->length);
}

kefir_result_t kefir_irblock_alloc(struct kefir_mem *mem, kefir_size_t capacity, struct kefir_irblock *block) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block pointer"));
    if (capacity == 0) {
        return kefir_irblock_init(block, NULL, 0);
    }
    void *area = KEFIR_MALLOC(mem, sizeof(struct kefir_irinstr) * capacity);
    REQUIRE(area != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory for IR block content"));
    kefir_result_t result = kefir_irblock_init(block, area, capacity);
    if (result != KEFIR_OK) {
        KEFIR_FREE(mem, area);
    }
    return result;
}

kefir_result_t kefir_irblock_realloc(struct kefir_mem *mem, kefir_size_t capacity, struct kefir_irblock *block) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL IR block pointer"));
    if (capacity == 0) {
        return kefir_irblock_free(mem, block);
    }
    REQUIRE(capacity >= block->length, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "New capacity cannot fit IR block length"));
    block->code = KEFIR_REALLOC(mem, block->code, sizeof(struct kefir_irinstr) * capacity);
    if (block->code == NULL) {
        block->length = 0;
        block->capacity = 0;
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE,
            "Failed to reallocste IR block");
    }
    block->capacity = capacity;
    return KEFIR_OK;
}

kefir_result_t kefir_irblock_free(struct kefir_mem *mem, struct kefir_irblock *block) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL IR block pointer"));
    if (block->code != NULL) {
        KEFIR_FREE(mem, block->code);
        block->code = NULL;
        block->capacity = 0;
        block->length = 0;
    }
    return KEFIR_OK;
}