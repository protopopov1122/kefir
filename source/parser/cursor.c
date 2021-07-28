/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/parser/cursor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_token_cursor_init(struct kefir_parser_token_cursor *cursor, struct kefir_token *tokens,
                                              kefir_size_t length) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected value token cursor"));
    REQUIRE(tokens != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token array"));

    cursor->tokens = tokens;
    cursor->length = length;
    cursor->index = 0;
    REQUIRE_OK(kefir_token_new_sentinel(&cursor->sentinel));
    return KEFIR_OK;
}

const struct kefir_token *kefir_parser_token_cursor_at(const struct kefir_parser_token_cursor *cursor,
                                                       kefir_size_t offset) {
    REQUIRE(cursor != NULL, NULL);

    kefir_size_t index = cursor->index + offset;
    if (index < cursor->length) {
        return &cursor->tokens[index];
    } else {
        return &cursor->sentinel;
    }
}

kefir_result_t kefir_parser_token_cursor_reset(struct kefir_parser_token_cursor *cursor) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected value token cursor"));
    cursor->index = 0;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_token_cursor_next(struct kefir_parser_token_cursor *cursor) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected value token cursor"));
    if (cursor->index < cursor->length) {
        cursor->index++;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_token_cursor_save(struct kefir_parser_token_cursor *cursor, kefir_size_t *value) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected value token cursor"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to index"));
    *value = cursor->index;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_token_cursor_restore(struct kefir_parser_token_cursor *cursor, kefir_size_t index) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected value token cursor"));
    REQUIRE(index <= cursor->length, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Expected valid index"));
    cursor->index = index;
    return KEFIR_OK;
}
