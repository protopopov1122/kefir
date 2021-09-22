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

#include "kefir/lexer/lexer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include <string.h>

kefir_result_t kefir_lexer_init(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_symbol_table *symbols,
                                struct kefir_lexer_source_cursor *cursor, const struct kefir_lexer_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer source cursor"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser context"));

    lexer->symbols = symbols;
    lexer->cursor = cursor;
    lexer->context = context;
    REQUIRE_OK(kefir_lexer_init_punctuators(mem, lexer));
    kefir_result_t res = kefir_lexer_init_keywords(mem, lexer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_trie_free(mem, &lexer->punctuators);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_free(struct kefir_mem *mem, struct kefir_lexer *lexer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));

    REQUIRE_OK(kefir_trie_free(mem, &lexer->keywords));
    REQUIRE_OK(kefir_trie_free(mem, &lexer->punctuators));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_apply(struct kefir_mem *mem, struct kefir_lexer *lexer, kefir_lexer_callback_fn_t callback,
                                 void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(callback != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid callback"));

    struct kefir_lexer_source_cursor_state state;
    REQUIRE_OK(kefir_lexer_source_cursor_save(lexer->cursor, &state));
    kefir_result_t res = callback(mem, lexer, payload);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_lexer_source_cursor_restore(lexer->cursor, &state));
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

static kefir_result_t lexer_next_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);
    REQUIRE_OK(kefir_lexer_cursor_match_whitespace(mem, lexer, NULL));
    struct kefir_source_location source_location = lexer->cursor->location;
    if (kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'\0') {
        REQUIRE_OK(kefir_token_new_sentinel(token));
    } else {
        kefir_result_t res = kefir_lexer_match_constant(mem, lexer, token);
        if (res == KEFIR_NO_MATCH) {
            res = kefir_lexer_match_string_literal(mem, lexer, token, true);
        }
        if (res == KEFIR_NO_MATCH) {
            res = kefir_lexer_match_identifier_or_keyword(mem, lexer, token);
        }
        if (res == KEFIR_NO_MATCH) {
            res = kefir_lexer_match_punctuator(mem, lexer, token);
        }

        if (res == KEFIR_NO_MATCH) {
            return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &source_location,
                                          "Expected constant, string literal, identifier, keyword or punctuator");
        } else {
            REQUIRE_OK(res);
        }
    }
    token->source_location = source_location;
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_next(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, lexer_next_impl, token));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_populate_buffer(struct kefir_mem *mem, struct kefir_token_buffer *buffer,
                                           struct kefir_lexer *lexer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));

    kefir_bool_t scan_tokens = true;
    while (scan_tokens) {
        struct kefir_token token = {0};
        REQUIRE_OK(kefir_lexer_next(mem, lexer, &token));
        REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
        scan_tokens = token.klass != KEFIR_TOKEN_SENTINEL;
    }
    return KEFIR_OK;
}
