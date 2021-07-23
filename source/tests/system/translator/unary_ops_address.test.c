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

#include <stdlib.h>
#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

#define LEN 128
int INTS[LEN];
double DOUBLES[LEN];

char CHARS[] = "Hello, world!";

int *int_offset(int);
char *char_offset(int);
double *double_offset(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = 0; i < LEN; i++) {
        INTS[i] = i * 2;
        DOUBLES[i] = 5.107 * i;
    }

    for (int i = 0; i < LEN; i++) {
        int *ptr = int_offset(i);
        ASSERT(ptr == &INTS[i]);
        ASSERT(*ptr == i * 2);

        double *ptr2 = double_offset(i);
        ASSERT(ptr2 == &DOUBLES[i]);
        ASSERT(DOUBLE_EQUALS(*ptr2, DOUBLES[i], DOUBLE_EPSILON));
    }
    for (int i = 0; i < (int) sizeof(CHARS); i++) {
        char *ptr = char_offset(i);
        ASSERT(ptr == &CHARS[i]);
        ASSERT(*ptr == CHARS[i]);
    }
    return EXIT_SUCCESS;
}
