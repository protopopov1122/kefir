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

kefir_result_t kefir_lexer_init(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_symbol_table *symbols,
                                struct kefir_lexer_source_cursor *cursor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));

    lexer->symbols = symbols;
    lexer->cursor = cursor;
    REQUIRE_OK(kefir_lexer_init_punctuators(mem, lexer));
    kefir_result_t res = kefir_lexer_init_keywords(mem, lexer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_trie_free(mem, &lexer->punctuators);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_free(struct kefir_mem *mem, struct kefir_lexer *lexer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));

    REQUIRE_OK(kefir_trie_free(mem, &lexer->keywords));
    REQUIRE_OK(kefir_trie_free(mem, &lexer->punctuators));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_apply(struct kefir_mem *mem, struct kefir_lexer *lexer, kefir_lexer_callback_fn_t callback,
                                 void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(callback != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid callback"));

    struct kefir_lexer_source_cursor_state state;
    REQUIRE_OK(kefir_lexer_source_cursor_save(lexer->cursor, &state));
    kefir_result_t res = callback(mem, lexer, payload);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_lexer_source_cursor_restore(lexer->cursor, &state));
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t lexer_next_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);
    REQUIRE_OK(kefir_lexer_cursor_skip_whitespaces(lexer->cursor));
    if (kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'\0') {
        REQUIRE_OK(kefir_token_new_sentinel(token));
    } else {
        kefir_result_t res = kefir_lexer_match_identifier_or_keyword(mem, lexer, token);
        REQUIRE(res == KEFIR_NO_MATCH, res);
        res = kefir_lexer_match_string_literal(mem, lexer, token);
        REQUIRE(res == KEFIR_NO_MATCH, res);
        REQUIRE_OK(kefir_lexer_match_punctuator(mem, lexer, token));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_next(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, lexer_next_impl, token));
    return KEFIR_OK;
}
