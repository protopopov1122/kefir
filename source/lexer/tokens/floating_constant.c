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
#include "kefir/core/string_buffer.h"

static kefir_result_t match_fractional_part(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                            struct kefir_string_buffer *strbuf) {
    kefir_size_t integer_digits = 0;
    kefir_size_t fraction_digits = 0;

    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    for (; kefir_isdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        integer_digits++;
    }

    if (chr == U'.') {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    } else {
        REQUIRE(integer_digits > 0 && (chr == U'e' || chr == U'E'),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match floating constant"));
        return KEFIR_OK;
    }
    chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    for (; kefir_isdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        fraction_digits++;
    }
    REQUIRE(integer_digits > 0 || fraction_digits > 0,
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match floating constant"));
    return KEFIR_OK;
}

static kefir_result_t match_exponent(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                     struct kefir_string_buffer *strbuf) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(chr == U'e' || chr == U'E', KEFIR_OK);
    REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    if (chr == U'+' || chr == U'-') {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    }

    struct kefir_source_location exponent_location = lexer->cursor->location;
    kefir_size_t exponent_digits = 0;
    for (chr = kefir_lexer_source_cursor_at(lexer->cursor, 0); kefir_isdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        exponent_digits++;
    }
    REQUIRE(exponent_digits > 0, KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &exponent_location,
                                                        "Expected one or more floating point exponent digits"));
    return KEFIR_OK;
}

static kefir_result_t match_suffix(struct kefir_lexer *lexer, kefir_bool_t *double_constant) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    *double_constant = true;
    switch (chr) {
        case U'l':
        case U'L':
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
            break;

        case U'f':
        case U'F':
            *double_constant = false;
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
            break;

        default:
            // Intentionally left blank
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t match_decimal_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token,
                                         struct kefir_string_buffer *strbuf) {
    kefir_bool_t double_constant = true;

    REQUIRE_OK(match_fractional_part(mem, lexer, strbuf));
    REQUIRE_OK(match_exponent(mem, lexer, strbuf));
    REQUIRE_OK(match_suffix(lexer, &double_constant));

    kefir_size_t length;
    const char *literal = kefir_string_buffer_value(strbuf, &length);
    if (double_constant) {
        kefir_float64_t value = strtod(literal, NULL);
        REQUIRE_OK(kefir_token_new_constant_double(value, token));
    } else {
        kefir_float32_t value = strtof(literal, NULL);
        REQUIRE_OK(kefir_token_new_constant_float(value, token));
    }
    return KEFIR_OK;
}

static kefir_result_t match_decimal_floating_constant(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                      struct kefir_token *token) {
    struct kefir_string_buffer strbuf;
    REQUIRE_OK(kefir_string_buffer_init(mem, &strbuf, KEFIR_STRING_BUFFER_MULTIBYTE));
    kefir_result_t res = match_decimal_impl(mem, lexer, token, &strbuf);
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

static kefir_result_t match_hexadecimal_fractional_part(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                        struct kefir_string_buffer *strbuf) {
    kefir_size_t integer_digits = 0;
    kefir_size_t fraction_digits = 0;
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    for (; kefir_ishexdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        integer_digits++;
    }

    if (chr == U'.') {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    } else {
        REQUIRE(integer_digits > 0 && (chr == U'p' || chr == U'P'),
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match hexdecimal floating constant"));
        return KEFIR_OK;
    }
    chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    for (; kefir_ishexdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        fraction_digits++;
    }
    REQUIRE(integer_digits > 0 || fraction_digits > 0,
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match hexadecimal floating constant"));
    return KEFIR_OK;
}

static kefir_result_t match_hexadecimal_exponent(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                 struct kefir_string_buffer *strbuf) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(chr == U'p' || chr == U'P', KEFIR_OK);
    REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    if (chr == U'+' || chr == U'-') {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    }

    struct kefir_source_location exponent_location = lexer->cursor->location;
    kefir_size_t exponent_digits = 0;
    for (chr = kefir_lexer_source_cursor_at(lexer->cursor, 0); kefir_isdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
        exponent_digits++;
    }
    REQUIRE(exponent_digits > 0, KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &exponent_location,
                                                        "Expected one or more floating point exponent digits"));
    return KEFIR_OK;
}

static kefir_result_t match_hexadecimal_impl(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                             struct kefir_token *token, struct kefir_string_buffer *strbuf) {
    kefir_bool_t double_constant = true;

    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    kefir_char32_t chr2 = kefir_lexer_source_cursor_at(lexer->cursor, 1);
    REQUIRE(chr == U'0' && (chr2 == U'x' || chr2 == U'X'),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match hexadecimal floating constant"));
    REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr));
    REQUIRE_OK(kefir_string_buffer_insert(mem, strbuf, chr2));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 2));
    REQUIRE_OK(match_hexadecimal_fractional_part(mem, lexer, strbuf));
    REQUIRE_OK(match_hexadecimal_exponent(mem, lexer, strbuf));
    REQUIRE_OK(match_suffix(lexer, &double_constant));

    kefir_size_t length;
    const char *literal = kefir_string_buffer_value(strbuf, &length);
    if (double_constant) {
        kefir_float64_t value = strtod(literal, NULL);
        REQUIRE_OK(kefir_token_new_constant_double(value, token));
    } else {
        kefir_float32_t value = strtof(literal, NULL);
        REQUIRE_OK(kefir_token_new_constant_float(value, token));
    }
    return KEFIR_OK;
}

static kefir_result_t match_hexadecimal_floating_constant(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                          struct kefir_token *token) {
    struct kefir_string_buffer strbuf;
    REQUIRE_OK(kefir_string_buffer_init(mem, &strbuf, KEFIR_STRING_BUFFER_MULTIBYTE));
    kefir_result_t res = match_hexadecimal_impl(mem, lexer, token, &strbuf);
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

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    UNUSED(mem);
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    kefir_result_t res = match_hexadecimal_floating_constant(mem, lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    REQUIRE_OK(match_decimal_floating_constant(mem, lexer, token));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_match_floating_constant(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                   struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
