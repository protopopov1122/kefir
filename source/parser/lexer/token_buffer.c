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

#include "kefir/parser/lexer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

#define INIT_CAPACITY 256
#define CAPACITY_INCREASE 256

kefir_result_t kefir_token_buffer_init(struct kefir_mem *mem, struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token buffer"));

    buffer->length = 0;
    buffer->capacity = INIT_CAPACITY;
    buffer->tokens = KEFIR_MALLOC(mem, sizeof(struct kefir_token) * buffer->capacity);
    REQUIRE(buffer->tokens != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate token buffer"));
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_free(struct kefir_mem *mem, struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token buffer"));

    for (kefir_size_t i = 0; i < buffer->length; i++) {
        REQUIRE_OK(kefir_token_free(mem, &buffer->tokens[i]));
    }
    KEFIR_FREE(mem, buffer->tokens);
    buffer->tokens = NULL;
    buffer->capacity = 0;
    buffer->length = 0;
    return KEFIR_OK;
}

static kefir_result_t ensure_size(struct kefir_mem *mem, struct kefir_token_buffer *buffer) {
    if (buffer->length + 1 == buffer->capacity) {
        kefir_size_t newCapacity = buffer->capacity + CAPACITY_INCREASE;
        struct kefir_token *newBuffer = KEFIR_MALLOC(mem, sizeof(struct kefir_token) * newCapacity);
        REQUIRE(newBuffer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to reallocate token buffer"));
        memcpy(newBuffer, buffer->tokens, sizeof(struct kefir_token) * buffer->length);
        KEFIR_FREE(mem, buffer->tokens);
        buffer->tokens = newBuffer;
        buffer->capacity = newCapacity;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_token_buffer_consume(struct kefir_mem *mem, struct kefir_token_buffer *buffer,
                                          struct kefir_lexer *lexer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token buffer"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));

    kefir_bool_t scan_tokens = true;
    while (scan_tokens) {
        REQUIRE_OK(ensure_size(mem, buffer));
        struct kefir_token *token = &buffer->tokens[buffer->length];
        REQUIRE_OK(kefir_lexer_next(mem, lexer, token));
        buffer->length++;
        scan_tokens = token->klass != KEFIR_TOKEN_SENTINEL;
    }
    return KEFIR_OK;
}
