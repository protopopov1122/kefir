/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char a : 3;
    char b : 5;
    short c : 7;
    long d : 20;
    char : 0;
    int e : 15;
};

void assign(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -4; i <= 3; i++) {
        struct param val = {0};
        assign(&val, i);
        ASSERT(val.a == i);
        ASSERT(val.b == i + 1);
        ASSERT(val.c == i + 2);
        ASSERT(val.d == i + 3);
        ASSERT(val.e == i + 4);
    }

    return EXIT_SUCCESS;
}
