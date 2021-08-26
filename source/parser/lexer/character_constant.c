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

static kefir_result_t next_character(struct kefir_lexer_source_cursor *cursor, kefir_int_t *value,
                                     kefir_bool_t *continueScan) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    if (chr == U'\\') {
        *continueScan = true;
        REQUIRE_OK(kefir_lexer_cursor_next_escape_sequence(cursor, &chr));
    } else if (chr == U'\'') {
        *continueScan = false;
    } else {
        *continueScan = true;
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    }

    if (*continueScan) {
        char multibyte[MB_CUR_MAX];
        mbstate_t mbstate = {0};
        size_t sz = c32rtomb(multibyte, chr, &mbstate);
        REQUIRE(sz != (size_t) -1,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to convert unicode character into multibyte"));
        char *iter = multibyte;
        while (sz--) {
            *value <<= 8;
            *value += *iter;
            iter++;
        }
    }
    return KEFIR_OK;
}
static kefir_result_t next_wide_character(struct kefir_lexer_source_cursor *cursor, kefir_char32_t *value,
                                          kefir_bool_t *continueScan) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    if (chr == U'\\') {
        *continueScan = true;
        REQUIRE_OK(kefir_lexer_cursor_next_escape_sequence(cursor, value));
    } else if (chr == U'\'') {
        *continueScan = false;
    } else {
        *continueScan = true;
        *value = chr;
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    }
    return KEFIR_OK;
}

static kefir_result_t match_narrow_character(struct kefir_lexer *lexer, struct kefir_token *token) {
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'\'',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match character constant"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));

    kefir_int_t character_value = 0;
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(chr != U'\'',
            KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Character constant shall contain at least one character"));
    for (kefir_bool_t scan = true; scan;) {
        REQUIRE_OK(next_character(lexer->cursor, &character_value, &scan));
    }
    chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(chr == U'\'', KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Character constant shall terminate with single quote"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    REQUIRE_OK(kefir_token_new_constant_char((kefir_int_t) character_value, token));
    return KEFIR_OK;
}

static kefir_result_t scan_wide_character(struct kefir_lexer *lexer, kefir_char32_t *value) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(chr != U'\'',
            KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Character constant shall contain at least one character"));
    for (kefir_bool_t scan = true; scan;) {
        REQUIRE_OK(next_wide_character(lexer->cursor, value, &scan));
    }
    chr = kefir_lexer_source_cursor_at(lexer->cursor, 0);
    REQUIRE(chr == U'\'', KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Character constant shall terminate with single quote"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
    return KEFIR_OK;
}

static kefir_result_t match_wide_character(struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(token);
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'L' &&
                kefir_lexer_source_cursor_at(lexer->cursor, 1) == U'\'',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match wide character constant"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 2));

    kefir_char32_t character_value = 0;
    REQUIRE_OK(scan_wide_character(lexer, &character_value));
    REQUIRE_OK(kefir_token_new_constant_wide_char((kefir_wchar_t) character_value, token));
    return KEFIR_OK;
}

static kefir_result_t match_unicode16_character(struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(token);
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'u' &&
                kefir_lexer_source_cursor_at(lexer->cursor, 1) == U'\'',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match unicode character constant"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 2));

    kefir_char32_t character_value = 0;
    REQUIRE_OK(scan_wide_character(lexer, &character_value));
    REQUIRE_OK(kefir_token_new_constant_unicode16_char((kefir_char16_t) character_value, token));
    return KEFIR_OK;
}

static kefir_result_t match_unicode32_character(struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(token);
    REQUIRE(kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'U' &&
                kefir_lexer_source_cursor_at(lexer->cursor, 1) == U'\'',
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match unicode character constant"));
    REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 2));

    kefir_char32_t character_value = 0;
    REQUIRE_OK(scan_wide_character(lexer, &character_value));
    REQUIRE_OK(kefir_token_new_constant_unicode32_char((kefir_char32_t) character_value, token));
    return KEFIR_OK;
}

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    UNUSED(mem);
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_token *, token, payload);

    kefir_result_t res = match_narrow_character(lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = match_wide_character(lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = match_unicode16_character(lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = match_unicode32_character(lexer, token);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match character constant");
}

kefir_result_t kefir_lexer_match_character_constant(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                    struct kefir_token *token) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token"));

    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, token));
    return KEFIR_OK;
}
