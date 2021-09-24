/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/lexer/buffer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

#define INIT_CAPACITY 256
#define CAPACITY_INCREASE 256

static kefir_result_t ensure_size(struct kefir_mem *mem, struct kefir_token_buffer *buffer) {
    if (buffer->length + 1 >= buffer->capacity) {
        kefir_size_t newCapacity = buffer->capacity == 0 ? INIT_CAPACITY : buffer->capacity + CAPACITY_INCREASE;
        struct kefir_token *newBuffer = KEFIR_MALLOC(mem, sizeof(struct kefir_token) * newCapacity);
        REQUIRE(newBuffer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to reallocate token buffer"));
        if (buffer->tokens != NULL) {
            memcpy(newBuffer, buffer->tokens, sizeof(struct kefir_token) * buffer->length);
            KEFIR_FREE(mem, buffer->tokens);
        }
        buffer->tokens = newBuffer;
        buffer->capacity = newCapacity;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_init(struct kefir_token_buffer *buffer) {
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    buffer->length = 0;
    buffer->capacity = 0;
    buffer->tokens = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_free(struct kefir_mem *mem, struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    for (kefir_size_t i = 0; i < buffer->length; i++) {
        REQUIRE_OK(kefir_token_free(mem, &buffer->tokens[i]));
    }
    KEFIR_FREE(mem, buffer->tokens);
    buffer->tokens = NULL;
    buffer->capacity = 0;
    buffer->length = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_emplace(struct kefir_mem *mem, struct kefir_token_buffer *buffer,
                                          struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    REQUIRE_OK(ensure_size(mem, buffer));
    REQUIRE_OK(kefir_token_move(&buffer->tokens[buffer->length], token));
    buffer->length++;
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_insert(struct kefir_mem *mem, struct kefir_token_buffer *dst,
                                         struct kefir_token_buffer *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination token buffer"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source token buffer"));

    for (kefir_size_t i = 0; i < src->length; i++) {
        REQUIRE_OK(kefir_token_buffer_emplace(mem, dst, &src->tokens[i]));
    }
    KEFIR_FREE(mem, src->tokens);
    src->tokens = NULL;
    src->length = 0;
    src->capacity = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_pop(struct kefir_mem *mem, struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(buffer->length > 0, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Cannot pop token from empty buffer"));

    REQUIRE_OK(kefir_token_free(mem, &buffer->tokens[buffer->length - 1]));
    buffer->length--;
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_copy(struct kefir_mem *mem, struct kefir_token_buffer *dst,
                                       const struct kefir_token_buffer *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination token buffer"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source token buffer"));

    for (kefir_size_t i = 0; i < src->length; i++) {
        struct kefir_token token_copy;
        REQUIRE_OK(kefir_token_copy(mem, &token_copy, &src->tokens[i]));
        kefir_result_t res = kefir_token_buffer_emplace(mem, dst, &token_copy);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_free(mem, &token_copy);
            return res;
        });
    }
    return KEFIR_OK;
}
