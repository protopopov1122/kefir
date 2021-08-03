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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 7;
    char b : 5;
    long c : 29;
    char d : 6;
    short e : 13;
    int f : 23;
    long g : 35;
    short h : 14;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -10; i < 10; i++) {
        for (int j = -10; j < 10; j++) {
            struct param val = {
                .a = j, .b = (j + 1) & 15, .c = j + 2, .d = j + 3, .e = j + 4, .f = j + 5, .g = j + 6, .h = j + 7};
            modify(&val, i);
            ASSERT(val.a == j - i);
            ASSERT((val.b & 15) == ((((j + 1) & 15) - i) & 15));
            ASSERT(val.c == j + 2 - i);
            ASSERT(val.d == j + 3 - i);
            ASSERT(val.e == j + 4 - i);
            ASSERT(val.f == j + 5 - i);
            ASSERT(val.g == j + 6 - i);
            ASSERT(val.h == j + 7 - i);
        }
    }

    return EXIT_SUCCESS;
}
