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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int get_int(int *, int);
char get_char(char *, int);
double get_double(double *, int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

#define LEN 128
    int INTS[LEN];
    char CHARS[] = "Hello, cruel world!";
    double DOUBLES[LEN];
    for (int i = 0; i < LEN; i++) {
        INTS[i] = ~i;
        DOUBLES[i] = 1.6549e8 * i;
    }

    for (int i = 0; i < LEN; i++) {
        ASSERT(get_int(INTS, i) == ~i);
        ASSERT(DOUBLE_EQUALS(get_double(DOUBLES, i), DOUBLES[i], DOUBLE_EPSILON));
    }
    for (int i = 0; i < (int) sizeof(CHARS); i++) {
        ASSERT(get_char(CHARS, i) == CHARS[i]);
    }
    return EXIT_SUCCESS;
}
