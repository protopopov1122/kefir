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

static kefir_result_t next_decimal_constant(struct kefir_lexer *lexer, kefir_uint64_t *value) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(kefir_isdigit32(chr) && chr != U'0',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match decimal integer constant"));

    *value = 0;
    for (; kefir_isdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        *value *= 10;
        *value += chr - U'0';
    }
    return KEFIR_OK;
}

static kefir_result_t next_hexadecimal_constant(struct kefir_lexer *lexer, kefir_uint64_t *value) {
    kefir_char32_t init_chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    kefir_char32_t init_chr2 = kefir_lexer_source_cursor_at(lexer->cursor, 1);
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 2);
    REQUIRE(init_chr == U'0' && (init_chr2 == U'x' || init_chr2 == U'X') && kefir_ishexdigit32(chr),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match hexadecimal integer constant"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 2));

    *value = 0;
    for (; kefir_ishexdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        *value <<= 4;
        *value += kefir_hex32todec(chr);
    }
    return KEFIR_OK;
}

static kefir_result_t next_octal_constant(struct kefir_lexer *lexer, kefir_uint64_t *value) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(chr == U'0', KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match octal integer constant"));

    *value = 0;
    for (; kefir_isoctdigit32(chr);
         kefir_lexer_source_cursor_next(lexer->cursor, 1), chr = kefir_lexer_source_cursor_at(lexer->cursor, 0)) {
        *value <<= 3;
        *value += chr - U'0';
    }
    return KEFIR_OK;
}

static kefir_result_t scan_suffix(struct kefir_lexer *lexer, kefir_uint64_t value, struct kefir_token *token) {
    static const struct Suffix {
        const kefir_char32_t *suffix;
        kefir_bool_t unsignedSuffix;
        kefir_bool_t longSuffix;
        kefir_bool_t longLongSuffix;
    } SUFFIXES[] = {
        {U"uLL", true, false, true}, {U"ull", true, false, true}, {U"LLu", true, false, true},
        {U"llu", true, false, true}, {U"ULL", true, false, true}, {U"Ull", true, false, true},
        {U"LLU", true, false, true}, {U"llU", true, false, true}, {U"uL", true, true, false},
        {U"ul", true, true, false},  {U"Lu", true, true, false},  {U"lu", true, true, false},
        {U"UL", true, true, false},  {U"Ul", true, true, false},  {U"LU", true, true, false},
        {U"lU", true, true, false},  {U"u", true, false, false},  {U"U", true, false, false},
        {U"LL", false, false, true}, {U"ll", false, false, true}, {U"L", false, true, false},
        {U"l", false, true, false},
    };
    static const kefir_size_t SUFFIXES_LENGTH = sizeof(SUFFIXES) / sizeof(SUFFIXES[0]);

    const struct Suffix *matchedSuffix = NULL;
    for (kefir_size_t i = 0; matchedSuffix == NULL && i < SUFFIXES_LENGTH; i++) {
        const struct Suffix *suffix = &SUFFIXES[i];
        kefir_result_t res = kefir_lexer_cursor_match_string(lexer->cursor, suffix->suffix);
        if (res == KEFIR_OK) {
            matchedSuffix = suffix;
            REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, kefir_strlen32(suffix->suffix)));
        } else {
            REQUIRE(res == KEFIR_NO_MATCH, res);
        }
    }

    if (matchedSuffix == NULL) {
        REQUIRE_OK(kefir_token_new_constant_int((kefir_int64_t) value, token));
    } else if (matchedSuffix->unsignedSuffix && matchedSuffix->longLongSuffix) {
        REQUIRE_OK(kefir_token_new_constant_ulong_long(value, token));
    } else if (matchedSuffix->unsignedSuffix && matchedSuffix->longSuffix) {
        REQUIRE_OK(kefir_token_new_constant_ulong(value, token));
    } else if (matchedSuffix->unsignedSuffix) {
        REQUIRE_OK(kefir_token_new_constant_uint(value, token));
    } else if (matchedSuffix->longLongSuffix) {
        REQUIRE_OK(kefir_token_new_constant_long_long((kefir_int64_t) value, token));
    } else if (matchedSuffix->longSuffix) {
        REQUIRE_OK(kefir_token_new_constant_long((kefir_int64_t) value, token));
    } else {
        REQUIRE_OK(kefir_token_new_constant_int((kefir_int64_t) value, token));
    }
    return KEFIR_OK;
}

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    UNUSED(mem);
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    kefir_uint64_t value;
    kefir_result_t res = next_decimal_constant(lexer, &value);
    if (res == KEFIR_NO_MATCH) {
        res = next_hexadecimal_constant(lexer, &value);
    }
    if (res == KEFIR_NO_MATCH) {
        res = next_octal_constant(lexer, &value);
    }
    if (res == KEFIR_NO_MATCH) {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match integer constant");
    }
    REQUIRE_OK(res);
    REQUIRE_OK(scan_suffix(lexer, value, token));
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_match_integer_constant(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                  struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
