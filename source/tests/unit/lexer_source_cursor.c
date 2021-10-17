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
#include "kefir/lexer/source_cursor.h"

DEFINE_CASE(lexer_source_cursor1, "Lexer - source cursor #1") {
    const char CONTENT[] = "123 {{(Hello\nworld\t,,,x!";
    kefir_size_t LENGTH = sizeof(CONTENT);

    struct kefir_lexer_source_cursor cursor;
    ASSERT_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, LENGTH, ""));

    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'1');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'2');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'3');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U' ');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'{');

    struct kefir_lexer_source_cursor_state state, state2;
    ASSERT_OK(kefir_lexer_source_cursor_save(&cursor, &state));
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'2');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'3');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U' ');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'(');

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
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
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'w');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'o');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'r');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'\t');

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 4));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'\t');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U',');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'x');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'!');

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
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

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state2));
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'H');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'o');

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'1');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'2');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'3');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U' ');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'{');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'{');
}
END_CASE

DEFINE_CASE(lexer_source_cursor_newlines, "Lexer - source cursor newlines") {
    const char CONTENT[] = "abc\\\ndef\\\n\\\n\\\n\\\nghi\njk\\\\l\n\n\n\\";
    kefir_size_t LENGTH = sizeof(CONTENT);

    struct kefir_lexer_source_cursor cursor;
    ASSERT_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, LENGTH, ""));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'a');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'b');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'c');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'f');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'g');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 7) == U'h');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 8) == U'i');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 9) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 10) == U'j');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 11) == U'k');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 12) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 13) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 14) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 15) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 16) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 17) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 18) == U'\\');
    ASSERT(cursor.location.line == 1);
    ASSERT(cursor.location.column == 1);

    struct kefir_lexer_source_cursor_state state;
    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 2));
    ASSERT_OK(kefir_lexer_source_cursor_save(&cursor, &state));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'c');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'f');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'g');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'h');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'i');
    ASSERT(cursor.location.line == 1);
    ASSERT(cursor.location.column == 3);

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'f');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'g');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'h');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'i');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'\n');
    ASSERT(cursor.location.line == 1);
    ASSERT(cursor.location.column == 4);

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 1));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'f');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'g');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'h');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'i');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'\n');
    ASSERT(cursor.location.line == 2);
    ASSERT(cursor.location.column == 2);

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'h');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'i');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'j');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'k');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 7) == U'l');
    ASSERT(cursor.location.line == 6);
    ASSERT(cursor.location.column == 2);

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 3));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'j');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'k');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 7) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 8) == U'\\');
    ASSERT(cursor.location.line == 7);
    ASSERT(cursor.location.column == 1);

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 5));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'\0');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'\0');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'\0');
    ASSERT(cursor.location.line == 7);
    ASSERT(cursor.location.column == 6);

    ASSERT_OK(kefir_lexer_source_cursor_next(&cursor, 5));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'\0');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'\0');

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state));
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 0) == U'c');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 1) == U'd');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 2) == U'e');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 3) == U'f');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 4) == U'g');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 5) == U'h');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 6) == U'i');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 7) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 8) == U'j');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 9) == U'k');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 10) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 11) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 12) == U'l');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 13) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 14) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 15) == U'\n');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 16) == U'\\');
    ASSERT(kefir_lexer_source_cursor_at(&cursor, 17) == U'\0');
    ASSERT(cursor.location.line == 1);
    ASSERT(cursor.location.column == 3);
}
END_CASE
