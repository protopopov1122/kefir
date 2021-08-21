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

static kefir_result_t match_narrow_unicode_string_sequence(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                           struct kefir_token *token) {
    UNUSED(mem);
    UNUSED(token);
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'u' &&
                kefir_lexer_source_cursor_at(lexer->cursor, 1) == U'8' &&
                kefir_lexer_source_cursor_at(lexer->cursor, 2) == U'\"',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match narrow unicode string literal"));

    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Narrow unicode string literals are not implemented yet");
}

static kefir_result_t match_wide_unicode_string_sequence(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                         struct kefir_token *token) {
    UNUSED(mem);
    UNUSED(token);
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'U' &&
                kefir_lexer_source_cursor_at(lexer->cursor, 1) == U'\"',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match wide unicode string literal"));

    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Wide unicode string literals are not implemented yet");
}

static kefir_result_t match_wide_string_sequence(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                 struct kefir_token *token) {
    UNUSED(mem);
    UNUSED(token);
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'L' &&
                kefir_lexer_source_cursor_at(lexer->cursor, 1) == U'\"',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match wide string literal"));

    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Wide string literals are not implemented yet");
}

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    kefir_result_t res = kefir_lexer_next_narrow_string_literal(mem, lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = match_narrow_unicode_string_sequence(mem, lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = match_wide_unicode_string_sequence(mem, lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = match_wide_string_sequence(mem, lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match string literal");
}

kefir_result_t kefir_lexer_match_string_literal(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
