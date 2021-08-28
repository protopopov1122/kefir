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
#include "kefir/parser/string_literal_impl.h"

static kefir_result_t try_convert_string(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                         struct kefir_string_buffer *buffer, const kefir_char32_t *prefix,
                                         kefir_string_buffer_mode_t newMode) {
    REQUIRE_OK(kefir_lexer_cursor_match_string(lexer->cursor, prefix));
    REQUIRE_OK(kefir_string_buffer_convert(mem, buffer, newMode));
    REQUIRE_OK(kefir_lexer_next_string_literal_sequence_impl(mem, lexer, prefix, buffer));
    return KEFIR_OK;
}

static kefir_result_t next_string_literal(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token,
                                          struct kefir_string_buffer *buffer) {
    REQUIRE_OK(kefir_lexer_next_string_literal_sequence_impl(mem, lexer, U"\"", buffer));
    kefir_size_t literal_length;
    const void *literal = NULL;

    kefir_result_t res = try_convert_string(mem, lexer, buffer, U"u8\"", KEFIR_STRING_BUFFER_UNICODE8);
    if (res == KEFIR_OK) {
        literal = kefir_string_buffer_value(buffer, &literal_length);
        REQUIRE_OK(kefir_token_new_string_literal_unicode8(mem, literal, literal_length, token));
        return KEFIR_OK;
    } else {
        REQUIRE(res == KEFIR_NO_MATCH, res);
    }

    res = try_convert_string(mem, lexer, buffer, U"u\"", KEFIR_STRING_BUFFER_UNICODE16);
    if (res == KEFIR_OK) {
        literal = kefir_string_buffer_value(buffer, &literal_length);
        REQUIRE_OK(kefir_token_new_string_literal_unicode16(mem, literal, literal_length, token));
        return KEFIR_OK;
    } else {
        REQUIRE(res == KEFIR_NO_MATCH, res);
    }

    res = try_convert_string(mem, lexer, buffer, U"U\"", KEFIR_STRING_BUFFER_UNICODE32);
    if (res == KEFIR_OK) {
        literal = kefir_string_buffer_value(buffer, &literal_length);
        REQUIRE_OK(kefir_token_new_string_literal_unicode32(mem, literal, literal_length, token));
        return KEFIR_OK;
    } else {
        REQUIRE(res == KEFIR_NO_MATCH, res);
    }

    res = try_convert_string(mem, lexer, buffer, U"L\"", KEFIR_STRING_BUFFER_WIDE);
    if (res == KEFIR_OK) {
        literal = kefir_string_buffer_value(buffer, &literal_length);
        REQUIRE_OK(kefir_token_new_string_literal_wide(mem, literal, literal_length, token));
        return KEFIR_OK;
    } else {
        REQUIRE(res == KEFIR_NO_MATCH, res);
    }

    literal = kefir_string_buffer_value(buffer, &literal_length);
    REQUIRE_OK(kefir_token_new_string_literal_multibyte(mem, literal, literal_length, token));
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
    REQUIRE_OK(kefir_string_buffer_init(mem, &string_buffer, KEFIR_STRING_BUFFER_MULTIBYTE));

    kefir_result_t res = next_string_literal(mem, lexer, token, &string_buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
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
