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
#include <stdarg.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct test {
    double f1;
    long f2;
};

struct test getarg(int, va_list);

struct test maketest(long base) {
    struct test val = {.f1 = (double) base, .f2 = base};
    return val;
}

struct test arg_getter(int count, ...) {
    va_list list;
    va_start(list, count);
    struct test val = getarg(count, list);
    va_end(list);
    return val;
}

struct test getter(int idx) {
    return arg_getter(idx, maketest(0), maketest(10), maketest(20), maketest(30), maketest(40), maketest(50),
                      maketest(60), maketest(70), maketest(80), maketest(90));
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = 0; i < 10; i++) {
        struct test val = getter(i);
        ASSERT(val.f1 == (double) i * 10);
        ASSERT(val.f2 == i * 10);
    }
    return EXIT_SUCCESS;
}
