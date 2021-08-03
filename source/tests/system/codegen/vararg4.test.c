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

struct test {
    long arr[4];
};

struct test getarg(int, ...);

struct test maketest(long base) {
    struct test val = {.arr = {base, base + 1, base + 10, base + 100}};
    return val;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        struct test arg = getarg(0, maketest(i - 1), maketest(i), maketest(i + 1));
        ASSERT(arg.arr[0] == i - 1);
        ASSERT(arg.arr[1] == i);
        ASSERT(arg.arr[2] == i + 9);
        ASSERT(arg.arr[3] == i + 99);
        arg = getarg(1, maketest(i - 1), maketest(i), maketest(i + 1));
        ASSERT(arg.arr[0] == i);
        ASSERT(arg.arr[1] == i + 1);
        ASSERT(arg.arr[2] == i + 10);
        ASSERT(arg.arr[3] == i + 100);
        arg = getarg(2, maketest(i - 1), maketest(i), maketest(i + 1));
        ASSERT(arg.arr[0] == i + 1);
        ASSERT(arg.arr[1] == i + 2);
        ASSERT(arg.arr[2] == i + 11);
        ASSERT(arg.arr[3] == i + 101);
    }
    return EXIT_SUCCESS;
}
