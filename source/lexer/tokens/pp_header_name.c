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
#include "kefir/util/char32.h"
#include "kefir/core/source_error.h"

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    kefir_char32_t terminator;
    if (chr == U'<') {
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        terminator = U'>';
    } else if (chr == U'\"') {
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        terminator = U'\"';
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match pp header name");
    }

    const char *begin = kefir_lexer_source_cursor_current(lexer->cursor);
    kefir_bool_t scan_pp_header_name = true;
    kefir_size_t header_name_length = 0;
    while (scan_pp_header_name) {
        chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
        REQUIRE(chr != lexer->context->newline,
                KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &lexer->cursor->location,
                                       "Unexpected newline in preprocessor header name"));
        scan_pp_header_name = chr != terminator;
        if (scan_pp_header_name) {
            header_name_length++;
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        }
    }

    const char *end = kefir_lexer_source_cursor_current(lexer->cursor);
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    REQUIRE(header_name_length > 0, KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &lexer->cursor->location,
                                                           "Preprocessor header name cannot be empty"));
    REQUIRE_OK(kefir_token_new_pp_header_name(mem, terminator == U'>', begin, end - begin, token));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_match_pp_header_name(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
