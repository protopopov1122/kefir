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

static kefir_result_t skip_whitespaces(struct kefir_lexer_source_cursor *cursor, kefir_bool_t *skip) {
    while (kefir_isspace32(kefir_lexer_source_cursor_at(cursor, 0))) {
        *skip = true;
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    }
    return KEFIR_OK;
}

static kefir_result_t skip_multiline_comment(struct kefir_lexer_source_cursor *cursor, kefir_bool_t *skip) {
    if (kefir_lexer_source_cursor_at(cursor, 0) == U'/' && kefir_lexer_source_cursor_at(cursor, 1) == U'*') {
        *skip = true;

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
                                           struct kefir_lexer_source_cursor *cursor, kefir_bool_t *skip) {
    if (kefir_lexer_source_cursor_at(cursor, 0) == U'/' && kefir_lexer_source_cursor_at(cursor, 1) == U'/') {
        *skip = true;

        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 2));
        while (kefir_lexer_source_cursor_at(cursor, 0) != context->newline &&
               kefir_lexer_source_cursor_at(cursor, 0) != U'\0') {
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
        }

        if (kefir_lexer_source_cursor_at(cursor, 0) == context->newline) {
            REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_lexer_cursor_skip_insignificant_chars(const struct kefir_lexer_context *context,
                                                           struct kefir_lexer_source_cursor *cursor) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser context"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer source cursor"));

    kefir_bool_t skip = true;
    while (skip) {
        skip = false;
        REQUIRE_OK(skip_whitespaces(cursor, &skip));
        REQUIRE_OK(skip_multiline_comment(cursor, &skip));
        REQUIRE_OK(skip_oneline_comment(context, cursor, &skip));
    }
    return KEFIR_OK;
}
