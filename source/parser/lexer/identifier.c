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
#include "kefir/core/source_error.h"

#define MAX_IDENTIFIER_LENGTH 64

static kefir_result_t scan_identifier_nondigit(struct kefir_lexer_source_cursor *cursor, kefir_char32_t *target) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    if (kefir_isnondigit32(chr)) {
        *target = chr;
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    } else {
        REQUIRE_OK(kefir_lexer_cursor_next_universal_character(cursor, target));
    }
    return KEFIR_OK;
}

static kefir_result_t scan_identifier_digit(struct kefir_lexer_source_cursor *cursor, kefir_char32_t *target) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    if (kefir_isdigit32(chr)) {
        *target = chr;
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    } else {
        REQUIRE_OK(scan_identifier_nondigit(cursor, target));
    }
    return KEFIR_OK;
}

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    struct kefir_source_location identifier_location = lexer->cursor->location;
    kefir_char32_t identifier[MAX_IDENTIFIER_LENGTH + 1] = {U'\0'};
    REQUIRE_OK(scan_identifier_nondigit(lexer->cursor, &identifier[0]));

    kefir_size_t length = 1;
    kefir_bool_t scan_identifier = true;
    for (; scan_identifier;) {
        kefir_char32_t chr;
        kefir_result_t res = scan_identifier_digit(lexer->cursor, &chr);
        if (res == KEFIR_NO_MATCH) {
            scan_identifier = false;
        } else {
            REQUIRE_OK(res);
            REQUIRE(length < MAX_IDENTIFIER_LENGTH, KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &identifier_location,
                                                                           "Identifier exceeds maximum length"));
            identifier[length++] = chr;
        }
    }

    kefir_keyword_token_t keyword;
    kefir_result_t res = kefir_lexer_get_keyword(lexer, identifier, &keyword);
    if (res == KEFIR_NO_MATCH) {
        char mb_identifier[MB_CUR_MAX * MAX_IDENTIFIER_LENGTH];
        char *mb_identifier_ptr = &mb_identifier[0];
        mbstate_t mbstate = {0};
        for (kefir_size_t i = 0; i <= length; i++) {
            size_t sz = c32rtomb(mb_identifier_ptr, identifier[i], &mbstate);
            REQUIRE(sz != ((size_t) -1),
                    KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unable to convert wide character to multibyte character"));
            mb_identifier_ptr += sz;
        }
        REQUIRE_OK(kefir_token_new_identifier(mem, lexer->symbols, mb_identifier, token));
    } else {
        REQUIRE_OK(res);
        REQUIRE_OK(kefir_token_new_keyword(keyword, token));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_match_identifier_or_keyword(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                       struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
