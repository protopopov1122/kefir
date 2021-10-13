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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uchar.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

const char32_t *string1();
const char32_t *string2();
const char32_t *string3();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    const char32_t LITERAL1[] = U"Hello, world!";
    const char32_t LITERAL2[] = U"\n\n\t\tHey there\'\"!\v\n";
    const char32_t LITERAL3[] = U"\0\0\0";

    ASSERT(memcmp(string1(), LITERAL1, sizeof(LITERAL1)) == 0);
    ASSERT(memcmp(string2(), LITERAL2, sizeof(LITERAL2)) == 0);
    ASSERT(memcmp(string3(), LITERAL3, sizeof(LITERAL3)) == 0);
    ASSERT(memcmp(string1(), LITERAL1, sizeof(LITERAL1)) == 0);
    ASSERT(memcmp(string3(), LITERAL3, sizeof(LITERAL3)) == 0);
    ASSERT(memcmp(string2(), LITERAL2, sizeof(LITERAL2)) == 0);
    return EXIT_SUCCESS;
}
