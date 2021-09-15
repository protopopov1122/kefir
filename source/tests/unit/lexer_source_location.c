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
#include <string.h>

#define ASSERT_LOCATION(_location, _source, _line, _column)  \
    do {                                                     \
        ASSERT(strcmp((_location)->source, (_source)) == 0); \
        ASSERT((_location)->line == (_line));                \
        ASSERT((_location)->column == (_column));            \
    } while (0)

#define ASSERT_NEXT(_cursor, _char, _source, _line, _column)                  \
    do {                                                                      \
        ASSERT(kefir_lexer_source_cursor_at((_cursor), 0) == (_char));        \
        ASSERT_LOCATION(&(_cursor)->location, (_source), (_line), (_column)); \
        ASSERT_OK(kefir_lexer_source_cursor_next((_cursor), 1));              \
    } while (0)

DEFINE_CASE(lexer_source_location1, "Lexer - source location #1") {
    const char CONTENT[] = "abc A B   C\n"
                           "test\n\n\n"
                           "\tH\t\tello";
    kefir_size_t LENGTH = sizeof(CONTENT);

    struct kefir_lexer_source_cursor cursor;
    ASSERT_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, LENGTH, "file.c"));

    struct kefir_lexer_source_cursor_state state, state2;

    ASSERT_NEXT(&cursor, U'a', "file.c", 1, 1);
    ASSERT_NEXT(&cursor, U'b', "file.c", 1, 2);
    ASSERT_NEXT(&cursor, U'c', "file.c", 1, 3);
    ASSERT_NEXT(&cursor, U' ', "file.c", 1, 4);
    ASSERT_NEXT(&cursor, U'A', "file.c", 1, 5);
    ASSERT_NEXT(&cursor, U' ', "file.c", 1, 6);
    ASSERT_NEXT(&cursor, U'B', "file.c", 1, 7);
    ASSERT_NEXT(&cursor, U' ', "file.c", 1, 8);
    ASSERT_NEXT(&cursor, U' ', "file.c", 1, 9);
    ASSERT_NEXT(&cursor, U' ', "file.c", 1, 10);
    ASSERT_OK(kefir_lexer_source_cursor_save(&cursor, &state));
    ASSERT_NEXT(&cursor, U'C', "file.c", 1, 11);
    ASSERT_NEXT(&cursor, U'\n', "file.c", 1, 12);
    ASSERT_NEXT(&cursor, U't', "file.c", 2, 1);
    ASSERT_NEXT(&cursor, U'e', "file.c", 2, 2);
    ASSERT_NEXT(&cursor, U's', "file.c", 2, 3);
    ASSERT_NEXT(&cursor, U't', "file.c", 2, 4);
    ASSERT_NEXT(&cursor, U'\n', "file.c", 2, 5);
    ASSERT_NEXT(&cursor, U'\n', "file.c", 3, 1);
    ASSERT_NEXT(&cursor, U'\n', "file.c", 4, 1);
    ASSERT_NEXT(&cursor, U'\t', "file.c", 5, 1);
    ASSERT_NEXT(&cursor, U'H', "file.c", 5, 2);
    ASSERT_NEXT(&cursor, U'\t', "file.c", 5, 3);
    ASSERT_OK(kefir_lexer_source_cursor_save(&cursor, &state2));
    ASSERT_NEXT(&cursor, U'\t', "file.c", 5, 4);
    ASSERT_NEXT(&cursor, U'e', "file.c", 5, 5);
    ASSERT_NEXT(&cursor, U'l', "file.c", 5, 6);
    ASSERT_NEXT(&cursor, U'l', "file.c", 5, 7);
    ASSERT_NEXT(&cursor, U'o', "file.c", 5, 8);
    ASSERT_NEXT(&cursor, U'\0', "file.c", 5, 9);
    ASSERT_NEXT(&cursor, U'\0', "file.c", 5, 9);
    ASSERT_NEXT(&cursor, U'\0', "file.c", 5, 9);
    ASSERT_NEXT(&cursor, U'\0', "file.c", 5, 9);

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state));
    ASSERT_NEXT(&cursor, U'C', "file.c", 1, 11);
    ASSERT_NEXT(&cursor, U'\n', "file.c", 1, 12);
    ASSERT_NEXT(&cursor, U't', "file.c", 2, 1);
    ASSERT_NEXT(&cursor, U'e', "file.c", 2, 2);

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state2));
    ASSERT_NEXT(&cursor, U'\t', "file.c", 5, 4);
    ASSERT_NEXT(&cursor, U'e', "file.c", 5, 5);
    ASSERT_NEXT(&cursor, U'l', "file.c", 5, 6);
    ASSERT_NEXT(&cursor, U'l', "file.c", 5, 7);

    ASSERT_OK(kefir_lexer_source_cursor_restore(&cursor, &state));
    ASSERT_NEXT(&cursor, U'C', "file.c", 1, 11);
    ASSERT_NEXT(&cursor, U'\n', "file.c", 1, 12);
    ASSERT_NEXT(&cursor, U't', "file.c", 2, 1);
    ASSERT_NEXT(&cursor, U'e', "file.c", 2, 2);
}
END_CASE
