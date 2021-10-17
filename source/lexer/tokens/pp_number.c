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
#include "kefir/core/string_buffer.h"

static kefir_result_t match_pp_number(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token,
                                      struct kefir_string_buffer *strbuf) {
    struct kefir_source_location location = lexer->cursor->location;
    kefir_char32_t chr1 = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    kefir_char32_t chr2 = kefir_lexer_source_cursor_at(lexer->cursor, 1);
    if (kefir_isdigit32(chr1)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr1));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    } else if (chr1 == U'.' && kefir_isdigit32(chr2)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr1));
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr2));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 2));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match pp number");
    }

    kefir_bool_t scan_pp_number = true;
    while (scan_pp_number) {
        chr1 = kefir_lexer_source_cursor_at(lexer->cursor, 0);
        chr2 = kefir_lexer_source_cursor_at(lexer->cursor, 1);
        if ((chr1 == U'e' || chr1 == U'E' || chr1 == U'p' || chr1 == U'P') && (chr2 == U'+' || chr2 == U'-')) {
            REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr1));
            REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr2));
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 2));
        } else if (kefir_isdigit32(chr1) || kefir_isnondigit32(chr1) || chr1 == U'.') {
            REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr1));
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        } else {
            kefir_char32_t chr;
            kefir_result_t res = kefir_lexer_cursor_next_universal_character(lexer->cursor, &chr);
            if (res == KEFIR_NO_MATCH) {
                scan_pp_number = false;
            } else {
                REQUIRE_OK(res);
                REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
            }
        }
    }

    kefir_size_t pp_number_length;
    const char *pp_number = kefir_string_buffer_value(strbuf, &pp_number_length);
    REQUIRE_OK(kefir_token_new_pp_number(mem, pp_number, pp_number_length, token));
    token->source_location = location;
    return KEFIR_OK;
}

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    struct kefir_string_buffer strbuf;
    REQUIRE_OK(kefir_string_buffer_init(mem, &strbuf, KEFIR_STRING_BUFFER_MULTIBYTE));
    kefir_result_t res = match_pp_number(mem, lexer, token, &strbuf);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_string_buffer_free(mem, &strbuf);
        return res;
    });
    res = kefir_string_buffer_free(mem, &strbuf);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, token);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_match_pp_number(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                           struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
