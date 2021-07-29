/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "kefir/ir/instr.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_irblock_init(struct kefir_irblock *bcblock, void *area, kefir_size_t capacity) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block pointer"));
    REQUIRE((area != NULL && capacity != 0) || (area == NULL && capacity == 0),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL content pointer for non-zero capacity IR block"));
    REQUIRE_OK(kefir_vector_init(&bcblock->content, sizeof(struct kefir_irinstr), area, capacity));
    return KEFIR_OK;
}

kefir_size_t kefir_irblock_available(const struct kefir_irblock *bcblock) {
    REQUIRE(bcblock != NULL, 0);
    return kefir_vector_available(&bcblock->content);
}

kefir_size_t kefir_irblock_length(const struct kefir_irblock *bcblock) {
    REQUIRE(bcblock != NULL, 0);
    return kefir_vector_length(&bcblock->content);
}

struct kefir_irinstr *kefir_irblock_at(const struct kefir_irblock *bcblock, kefir_size_t idx) {
    REQUIRE(bcblock != NULL, NULL);
    return (struct kefir_irinstr *) kefir_vector_at(&bcblock->content, idx);
}

kefir_result_t kefir_irblock_appendi64(struct kefir_irblock *bcblock, kefir_iropcode_t opcode, kefir_int64_t arg) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    struct kefir_irinstr instr = {.opcode = opcode, .arg = {.i64 = arg}};
    return kefir_vector_append(&bcblock->content, &instr);
}

kefir_result_t kefir_irblock_appendu64(struct kefir_irblock *bcblock, kefir_iropcode_t opcode, kefir_uint64_t arg) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    struct kefir_irinstr instr = {.opcode = opcode, .arg = {.u64 = arg}};
    return kefir_vector_append(&bcblock->content, &instr);
}

kefir_result_t kefir_irblock_appendi32(struct kefir_irblock *bcblock, kefir_iropcode_t opcode, kefir_int32_t arg1,
                                       kefir_int32_t arg2) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    struct kefir_irinstr instr = {.opcode = opcode, .arg = {.i32 = {arg1, arg2}}};
    return kefir_vector_append(&bcblock->content, &instr);
}

kefir_result_t kefir_irblock_appendu32(struct kefir_irblock *bcblock, kefir_iropcode_t opcode, kefir_uint32_t arg1,
                                       kefir_uint32_t arg2) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    struct kefir_irinstr instr = {.opcode = opcode, .arg = {.u32 = {arg1, arg2}}};
    return kefir_vector_append(&bcblock->content, &instr);
}

kefir_result_t kefir_irblock_appendf64(struct kefir_irblock *bcblock, kefir_iropcode_t opcode, kefir_float64_t arg) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    struct kefir_irinstr instr = {.opcode = opcode, .arg = {.f64 = arg}};
    return kefir_vector_append(&bcblock->content, &instr);
}

kefir_result_t kefir_irblock_appendf32(struct kefir_irblock *bcblock, kefir_iropcode_t opcode, kefir_float32_t arg1,
                                       kefir_float32_t arg2) {
    REQUIRE(bcblock != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block"));
    struct kefir_irinstr instr = {.opcode = opcode, .arg = {.f32 = {arg1, arg2}}};
    return kefir_vector_append(&bcblock->content, &instr);
}

kefir_result_t kefir_irblock_copy(struct kefir_irblock *dst, const struct kefir_irblock *src) {
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL destination IR block pointer"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL source IR block pointer"));
    return kefir_vector_copy(&dst->content, src->content.content, src->content.length);
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
    REQUIRE_OK(kefir_vector_realloc(mem, capacity, &block->content));
    return KEFIR_OK;
}

kefir_result_t kefir_irblock_free(struct kefir_mem *mem, struct kefir_irblock *block) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(block != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected non-NULL IR block pointer"));
    REQUIRE_OK(kefir_vector_free(mem, &block->content));
    return KEFIR_OK;
}
