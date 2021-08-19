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

kefir_result_t kefir_lexer_cursor_skip_whitespaces(struct kefir_lexer_source_cursor *cursor) {
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid lexer source cursor"));

    while (kefir_isspace32(kefir_lexer_source_cursor_at(cursor, 0))) {
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
    }
    return KEFIR_OK;
}
