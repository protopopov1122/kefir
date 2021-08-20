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

#define MAX_IDENTIFIER_LENGTH 64

static kefir_uint32_t hex_to_digit(kefir_char32_t chr) {
    switch (chr) {
        case U'a':
        case U'A':
            return 10;

        case U'b':
        case U'B':
            return 11;

        case U'c':
        case U'C':
            return 12;

        case U'd':
        case U'D':
            return 13;

        case U'e':
        case U'E':
            return 14;

        case U'f':
        case U'F':
            return 15;

        default:
            if (chr >= U'0' && chr <= U'9') {
                return chr - U'0';
            } else {
                return ~((kefir_uint32_t) 0);
            }
    }
}

static kefir_result_t scan_identifier_nondigit(struct kefir_lexer_source_cursor *cursor, kefir_char32_t *target) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    kefir_char32_t chr2 = kefir_lexer_source_cursor_at(cursor, 1);
    if (kefir_isnondigit32(chr)) {
        *target = chr;
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    } else if (chr == U'\\' && chr2 == U'u') {
        kefir_char32_t hex1 = kefir_lexer_source_cursor_at(cursor, 2), hex2 = kefir_lexer_source_cursor_at(cursor, 3),
                       hex3 = kefir_lexer_source_cursor_at(cursor, 4), hex4 = kefir_lexer_source_cursor_at(cursor, 5);
        REQUIRE(kefir_ishexdigit32(hex1) && kefir_ishexdigit32(hex2) && kefir_ishexdigit32(hex3) &&
                    kefir_ishexdigit32(hex4),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match identifier"));
        *target =
            hex_to_digit(hex4) | (hex_to_digit(hex3) << 4) | (hex_to_digit(hex2) << 8) | (hex_to_digit(hex1) << 12);
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 6));
    } else {
        REQUIRE(chr == U'\\' && chr2 == U'U', KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match identifier"));
        kefir_char32_t hex1 = kefir_lexer_source_cursor_at(cursor, 2), hex2 = kefir_lexer_source_cursor_at(cursor, 3),
                       hex3 = kefir_lexer_source_cursor_at(cursor, 4), hex4 = kefir_lexer_source_cursor_at(cursor, 5),
                       hex5 = kefir_lexer_source_cursor_at(cursor, 6), hex6 = kefir_lexer_source_cursor_at(cursor, 7),
                       hex7 = kefir_lexer_source_cursor_at(cursor, 8), hex8 = kefir_lexer_source_cursor_at(cursor, 9);
        REQUIRE(kefir_ishexdigit32(hex1) && kefir_ishexdigit32(hex2) && kefir_ishexdigit32(hex3) &&
                    kefir_ishexdigit32(hex4) && kefir_ishexdigit32(hex5) && kefir_ishexdigit32(hex6) &&
                    kefir_ishexdigit32(hex7) && kefir_ishexdigit32(hex8),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match identifier"));
        *target = hex_to_digit(hex8) | (hex_to_digit(hex7) << 4) | (hex_to_digit(hex6) << 8) |
                  (hex_to_digit(hex5) << 12) | (hex_to_digit(hex4) << 16) | (hex_to_digit(hex3) << 20) |
                  (hex_to_digit(hex2) << 24) | (hex_to_digit(hex1) << 28);
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 10));
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
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

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
            REQUIRE(length < MAX_IDENTIFIER_LENGTH,
                    KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Identifier exceeds maximum length"));
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
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to convert wide character to multibyte character"));
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
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
