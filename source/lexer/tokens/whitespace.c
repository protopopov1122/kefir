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
#include "kefir/core/source_error.h"
#include "kefir/util/char32.h"

enum whitespace_match { WHITESPACE_NO_MATCH = 0, WHITESPACE_MATCH, WHITESPACE_NEWLINE };

static kefir_result_t skip_whitespaces(const struct kefir_lexer_context *context,
                                       struct kefir_lexer_source_cursor *cursor, enum whitespace_match *match) {
    kefir_char32_t chr;
    if (kefir_isspace32((chr = kefir_lexer_source_cursor_at(cursor, 0)))) {
        if (chr == context->newline) {
            *match = MAX(*match, WHITESPACE_NEWLINE);
        } else {
            *match = MAX(*match, WHITESPACE_MATCH);
        }
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    }
    return KEFIR_OK;
}

static kefir_result_t skip_multiline_comment(struct kefir_lexer_source_cursor *cursor, enum whitespace_match *match) {
    if (kefir_lexer_source_cursor_at(cursor, 0) == U'/' && kefir_lexer_source_cursor_at(cursor, 1) == U'*') {
        *match = MAX(*match, WHITESPACE_MATCH);

        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
        while (!(kefir_lexer_source_cursor_at(cursor, 0) == U'*' && kefir_lexer_source_cursor_at(cursor, 1) == U'/') &&
               kefir_lexer_source_cursor_at(cursor, 0) != U'\0') {
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
        }
        REQUIRE((kefir_lexer_source_cursor_at(cursor, 0) == U'*' && kefir_lexer_source_cursor_at(cursor, 1) == U'/'),
                KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &cursor->location, "Expected end of multiline comment"));
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
    }
    return KEFIR_OK;
}

static kefir_result_t skip_oneline_comment(const struct kefir_lexer_context *context,
                                           struct kefir_lexer_source_cursor *cursor, enum whitespace_match *match) {
    if (kefir_lexer_source_cursor_at(cursor, 0) == U'/' && kefir_lexer_source_cursor_at(cursor, 1) == U'/') {
        *match = MAX(*match, WHITESPACE_MATCH);

        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
        while (kefir_lexer_source_cursor_at(cursor, 0) != context->newline &&
               kefir_lexer_source_cursor_at(cursor, 0) != U'\0') {
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
        }

        if (kefir_lexer_source_cursor_at(cursor, 0) == context->newline) {
            *match = MAX(*match, WHITESPACE_NEWLINE);
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_cursor_match_whitespace(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                   struct kefir_token *token) {
    UNUSED(mem);
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));

    kefir_bool_t continue_scan = true;
    enum whitespace_match match = WHITESPACE_NO_MATCH;
    while (continue_scan) {
        enum whitespace_match matched = WHITESPACE_NO_MATCH;
        REQUIRE_OK(skip_whitespaces(lexer->context, lexer->cursor, &matched));
        REQUIRE_OK(skip_multiline_comment(lexer->cursor, &matched));
        REQUIRE_OK(skip_oneline_comment(lexer->context, lexer->cursor, &matched));
        continue_scan = matched != WHITESPACE_NO_MATCH && token == NULL;
        match = MAX(match, matched);
    }

    if (token != NULL) {
        REQUIRE(match != WHITESPACE_NO_MATCH, KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match whitespace"));
        REQUIRE_OK(kefir_token_new_pp_whitespace(match == WHITESPACE_NEWLINE, token));
    }
    return KEFIR_OK;
}
