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

#include "kefir/parser/lexem.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

kefir_result_t kefir_token_new_sentinel(struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_SENTINEL;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_keyword(kefir_keyword_token_t keyword, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_KEYWORD;
    token->keyword = keyword;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_identifier(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                          const char *identifier, struct kefir_token *token) {
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    REQUIRE((mem == NULL && symbols == NULL) || (mem != NULL && symbols != NULL),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                            "Expected both memory allocator and symbol table to be either valid or not"));

    if (symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to insert identifier into symbol table"));
    }
    token->klass = KEFIR_TOKEN_IDENTIFIER;
    token->identifier = identifier;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_int(kefir_int64_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_INTEGER;
    token->constant.integer = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_uint(kefir_uint64_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_UNSIGNED_INTEGER;
    token->constant.uinteger = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_long(kefir_int64_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_LONG_INTEGER;
    token->constant.integer = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_ulong(kefir_uint64_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_INTEGER;
    token->constant.uinteger = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_long_long(kefir_int64_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_LONG_LONG_INTEGER;
    token->constant.integer = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_ulong_long(kefir_uint64_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_LONG_INTEGER;
    token->constant.integer = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_char(kefir_int_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_CHAR;
    token->constant.character = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_char32(kefir_char32_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_UCHAR;
    token->constant.unicode_char = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_float(kefir_float32_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_FLOAT;
    token->constant.float32 = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_constant_double(kefir_float64_t value, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_CONSTANT;
    token->constant.type = KEFIR_CONSTANT_TOKEN_DOUBLE;
    token->constant.float64 = value;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_string_literal(struct kefir_mem *mem, const char *content, kefir_size_t length,
                                              struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid string literal"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));

    char *dest_content = KEFIR_MALLOC(mem, length);
    REQUIRE(dest_content != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate string literal"));
    memcpy(dest_content, content, length);
    token->klass = KEFIR_TOKEN_STRING_LITERAL;
    token->string_literal.content = dest_content;
    token->string_literal.length = length;
    return KEFIR_OK;
}

kefir_result_t kefir_token_new_punctuator(kefir_punctuator_token_t punctuator, struct kefir_token *token) {
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    token->klass = KEFIR_TOKEN_PUNCTUATOR;
    token->punctuator = punctuator;
    return KEFIR_OK;
}

kefir_result_t kefir_token_move(struct kefir_token *dst, struct kefir_token *src) {
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to destination token"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to source token"));
    *dst = *src;
    if (src->klass == KEFIR_TOKEN_STRING_LITERAL) {
        src->string_literal.content = NULL;
        src->string_literal.length = 0;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_token_copy(struct kefir_mem *mem, struct kefir_token *dst, const struct kefir_token *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to destination token"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to source token"));

    *dst = *src;
    if (src->klass == KEFIR_TOKEN_STRING_LITERAL) {
        char *content = KEFIR_MALLOC(mem, src->string_literal.length);
        REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate string literal"));
        memcpy(content, src->string_literal.content, src->string_literal.length);
        dst->string_literal.content = content;
        dst->string_literal.length = src->string_literal.length;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_token_free(struct kefir_mem *mem, struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));
    switch (token->klass) {
        case KEFIR_TOKEN_STRING_LITERAL:
            KEFIR_FREE(mem, (void *) token->string_literal.content);
            token->string_literal.content = NULL;
            token->string_literal.length = 0;
            break;

        case KEFIR_TOKEN_SENTINEL:
        case KEFIR_TOKEN_KEYWORD:
        case KEFIR_TOKEN_IDENTIFIER:
        case KEFIR_TOKEN_CONSTANT:
        case KEFIR_TOKEN_PUNCTUATOR:
            break;
    }
    return KEFIR_OK;
}
