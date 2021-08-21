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
#include "kefir/util/char32.h"
#include "kefir/core/string_buffer.h"

static kefir_result_t match_narrow_string(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                          struct kefir_string_buffer *buffer) {
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'\"',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match narrow string literal"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));

    for (kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0); chr != U'\"';
         chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {

        REQUIRE(chr != U'\0', KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Unexpected newline character"));
        REQUIRE(chr != U'\n', KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Unexpected null character"));
        if (chr == U'\\') {
            kefir_char32_t result;
            REQUIRE_OK(kefir_lexer_cursor_next_escape_sequence(lexer->cursor, &result));
            REQUIRE_OK(kefir_string_buffer_insert32(mem, buffer, result));
        } else {
            REQUIRE_OK(kefir_string_buffer_insert32(mem, buffer, chr));
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        }
    }
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    return KEFIR_OK;
}

static kefir_result_t match_narrow_string_sequence_impl(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                        struct kefir_string_buffer *buffer, struct kefir_token *token) {
    REQUIRE_OK(match_narrow_string(mem, lexer, buffer));
    kefir_result_t res = KEFIR_OK;
    while (res == KEFIR_OK) {
        REQUIRE_CHAIN(&res, kefir_lexer_cursor_skip_whitespaces(lexer->cursor));
        REQUIRE_CHAIN(&res, match_narrow_string(mem, lexer, buffer));
    }
    REQUIRE(res == KEFIR_NO_MATCH, res);

    kefir_size_t literal_length;
    const char *literal = kefir_string_buffer_value(buffer, &literal_length);
    REQUIRE_OK(kefir_token_new_string_literal(mem, literal, literal_length, token));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_next_narrow_string_literal(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                      struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token"));

    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'\"',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match narrow string literal"));

    struct kefir_string_buffer string_buffer;
    REQUIRE_OK(kefir_string_buffer_init(mem, &string_buffer));

    kefir_result_t res = match_narrow_string_sequence_impl(mem, lexer, &string_buffer, token);
    REQUIRE_ELSE(res == KEFIR_NO_MATCH, {
        kefir_string_buffer_free(mem, &string_buffer);
        return res;
    });

    res = kefir_string_buffer_free(mem, &string_buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, token);
        return res;
    });
    return KEFIR_OK;
}
