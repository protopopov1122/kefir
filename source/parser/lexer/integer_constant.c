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

enum integer_constant_type {
    CONSTANT_INT,
    CONSTANT_UNSIGNED_INT,
    CONSTANT_LONG,
    CONSTANT_UNSIGNED_LONG,
    CONSTANT_LONG_LONG,
    CONSTANT_UNSIGNED_LONG_LONG
};

static kefir_result_t get_permitted_constant_types(enum integer_constant_type original, kefir_bool_t decimal,
                                                   const enum integer_constant_type **permitted, kefir_size_t *length) {
    switch (original) {
        case CONSTANT_INT:
            if (decimal) {
                static const enum integer_constant_type types[] = {CONSTANT_INT, CONSTANT_LONG, CONSTANT_LONG_LONG};
                *permitted = types;
                *length = sizeof(types) / sizeof(types[0]);
            } else {
                static const enum integer_constant_type types[] = {CONSTANT_INT,       CONSTANT_UNSIGNED_INT,
                                                                   CONSTANT_LONG,      CONSTANT_UNSIGNED_LONG,
                                                                   CONSTANT_LONG_LONG, CONSTANT_UNSIGNED_LONG_LONG};
                *permitted = types;
                *length = sizeof(types) / sizeof(types[0]);
            }
            break;

        case CONSTANT_UNSIGNED_INT: {
            static const enum integer_constant_type types[] = {CONSTANT_UNSIGNED_INT, CONSTANT_UNSIGNED_LONG,
                                                               CONSTANT_UNSIGNED_LONG_LONG};
            *permitted = types;
            *length = sizeof(types) / sizeof(types[0]);
        } break;

        case CONSTANT_LONG:
            if (decimal) {
                static const enum integer_constant_type types[] = {CONSTANT_LONG, CONSTANT_LONG_LONG};
                *permitted = types;
                *length = sizeof(types) / sizeof(types[0]);
            } else {
                static const enum integer_constant_type types[] = {CONSTANT_LONG, CONSTANT_UNSIGNED_LONG,
                                                                   CONSTANT_LONG_LONG, CONSTANT_UNSIGNED_LONG_LONG};
                *permitted = types;
                *length = sizeof(types) / sizeof(types[0]);
            }
            break;

        case CONSTANT_UNSIGNED_LONG: {
            static const enum integer_constant_type types[] = {CONSTANT_UNSIGNED_LONG, CONSTANT_UNSIGNED_LONG_LONG};
            *permitted = types;
            *length = sizeof(types) / sizeof(types[0]);
        } break;

        case CONSTANT_LONG_LONG:
            if (decimal) {
                static const enum integer_constant_type types[] = {CONSTANT_LONG_LONG};
                *permitted = types;
                *length = sizeof(types) / sizeof(types[0]);
            } else {
                static const enum integer_constant_type types[] = {CONSTANT_LONG_LONG, CONSTANT_UNSIGNED_LONG_LONG};
                *permitted = types;
                *length = sizeof(types) / sizeof(types[0]);
            }
            break;

        case CONSTANT_UNSIGNED_LONG_LONG: {
            static const enum integer_constant_type types[] = {CONSTANT_UNSIGNED_LONG_LONG};
            *permitted = types;
            *length = sizeof(types) / sizeof(types[0]);
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t make_integral_constant(const struct kefir_parser_integral_types *integral_types,
                                             enum integer_constant_type type, kefir_uint64_t value,
                                             struct kefir_token *token) {
    switch (type) {
        case CONSTANT_INT:
            REQUIRE(value <= integral_types->integer_max_value,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided constant exceeds maximum value of its type"));
            REQUIRE_OK(kefir_token_new_constant_int((kefir_int64_t) value, token));
            break;

        case CONSTANT_UNSIGNED_INT:
            REQUIRE(value <= integral_types->uinteger_max_value,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided constant exceeds maximum value of its type"));
            REQUIRE_OK(kefir_token_new_constant_uint(value, token));
            break;

        case CONSTANT_LONG:
            REQUIRE(value <= integral_types->long_max_value,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided constant exceeds maximum value of its type"));
            REQUIRE_OK(kefir_token_new_constant_long((kefir_int64_t) value, token));
            break;

        case CONSTANT_UNSIGNED_LONG:
            REQUIRE(value <= integral_types->ulong_max_value,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided constant exceeds maximum value of its type"));
            REQUIRE_OK(kefir_token_new_constant_ulong(value, token));
            break;

        case CONSTANT_LONG_LONG:
            REQUIRE(value <= integral_types->long_long_max_value,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided constant exceeds maximum value of its type"));
            REQUIRE_OK(kefir_token_new_constant_long_long((kefir_int64_t) value, token));
            break;

        case CONSTANT_UNSIGNED_LONG_LONG:
            REQUIRE(value <= integral_types->ulong_long_max_value,
                    KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided constant exceeds maximum value of its type"));
            REQUIRE_OK(kefir_token_new_constant_ulong_long(value, token));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t build_integral_constant(const struct kefir_parser_integral_types *integral_types,
                                              enum integer_constant_type type, kefir_bool_t decimal,
                                              kefir_uint64_t value, struct kefir_token *token) {
    const enum integer_constant_type *permitted_types = NULL;
    kefir_size_t permitted_types_length = 0;
    REQUIRE_OK(get_permitted_constant_types(type, decimal, &permitted_types, &permitted_types_length));
    for (kefir_size_t i = 0; i < permitted_types_length; i++) {
        kefir_result_t res = make_integral_constant(integral_types, permitted_types[i], value, token);
        if (res == KEFIR_OK) {
            return KEFIR_OK;
        } else {
            REQUIRE(res == KEFIR_OUT_OF_BOUNDS, res);
        }
    }
    return KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided constant exceeds maximum value of its type");
}

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

static kefir_result_t scan_suffix(struct kefir_lexer *lexer, kefir_bool_t decimal, kefir_uint64_t value,
                                  struct kefir_token *token) {
    static const struct Suffix {
        const kefir_char32_t *suffix;
        enum integer_constant_type type;
    } SUFFIXES[] = {
        {U"uLL", CONSTANT_UNSIGNED_LONG_LONG},
        {U"ull", CONSTANT_UNSIGNED_LONG_LONG},
        {U"LLu", CONSTANT_UNSIGNED_LONG_LONG},
        {U"llu", CONSTANT_UNSIGNED_LONG_LONG},
        {U"ULL", CONSTANT_UNSIGNED_LONG_LONG},
        {U"Ull", CONSTANT_UNSIGNED_LONG_LONG},
        {U"LLU", CONSTANT_UNSIGNED_LONG_LONG},
        {U"llU", CONSTANT_UNSIGNED_LONG_LONG},
        {U"uL", CONSTANT_UNSIGNED_LONG},
        {U"ul", CONSTANT_UNSIGNED_LONG},
        {U"Lu", CONSTANT_UNSIGNED_LONG},
        {U"lu", CONSTANT_UNSIGNED_LONG},
        {U"UL", CONSTANT_UNSIGNED_LONG},
        {U"Ul", CONSTANT_UNSIGNED_LONG},
        {U"LU", CONSTANT_UNSIGNED_LONG},
        {U"lU", CONSTANT_UNSIGNED_LONG},
        {U"u", CONSTANT_UNSIGNED_INT},
        {U"U", CONSTANT_UNSIGNED_INT},
        {U"LL", CONSTANT_LONG_LONG},
        {U"ll", CONSTANT_LONG_LONG},
        {U"L", CONSTANT_LONG},
        {U"l", CONSTANT_LONG},
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
        REQUIRE_OK(build_integral_constant(lexer->integral_types, CONSTANT_INT, decimal, value, token));
    } else {
        REQUIRE_OK(build_integral_constant(lexer->integral_types, matchedSuffix->type, decimal, value, token));
    }
    return KEFIR_OK;
}

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    UNUSED(mem);
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    kefir_uint64_t value;
    kefir_bool_t decimal = true;
    kefir_result_t res = next_decimal_constant(lexer, &value);
    if (res == KEFIR_NO_MATCH) {
        decimal = false;
        res = next_hexadecimal_constant(lexer, &value);
    }
    if (res == KEFIR_NO_MATCH) {
        res = next_octal_constant(lexer, &value);
    }
    if (res == KEFIR_NO_MATCH) {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match integer constant");
    }
    REQUIRE_OK(res);
    REQUIRE_OK(scan_suffix(lexer, decimal, value, token));
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
