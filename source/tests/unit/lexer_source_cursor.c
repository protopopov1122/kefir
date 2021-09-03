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

#include "kefir/test/unit_test.h"
#include "kefir/parser/source_cursor.h"

DEFINE_CASE(lexer_source_cursor1, "Lexer - source cursor #1") {
    const char CONTENT[] = "123 {{(Hello\nworld\t,,,x!";
    kefir_size_t LENGTH = sizeof(CONTENT);

    struct kefir_lexer_source_cursor cursor;
    ASSERT_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, LENGTH, ""));
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);

    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'1');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'2');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'3');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U' ');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'{');

    struct kefir_lexer_source_cursor_state state, state2;
    ASSERT_OK(kefir_lexer_source_cursor_save(&cursor, &state));
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT + 1);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'2');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'3');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U' ');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'(');

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT + 4);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'(');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'H');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 7) == U'o');

    ASSERT_OK(kefir_lexer_source_cursor_save(&cursor, &state2));
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 9));
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT + 13);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'w');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'o');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'r');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'\t');

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 4));
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT + 17);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'\t');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'x');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'!');

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT + 20);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'x');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'!');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'\0');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'\0');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'\0');

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U',');
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'x');
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'!');
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'\0');
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'\0');
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 10));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'\0');
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT + LENGTH - 1);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state2));
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT + 7);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'H');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'o');

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state));
    ASSERT(kefir_lexer_source_cursor_begin(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_current(&cursor) == CONTENT);
    ASSERT(kefir_lexer_source_cursor_end(&cursor) == CONTENT + LENGTH);
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'1');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'2');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'3');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U' ');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'{');
}
END_CASE
