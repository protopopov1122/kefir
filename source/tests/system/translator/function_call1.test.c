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
#include <stdarg.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long sumall(int count, ...) {
    va_list args;
    va_start(args, count);
    long sum = 0;
    while (count--) {
        long arg = va_arg(args, long);
        sum += arg;
    }
    va_end(args);
    return sum;
}

long sum1(long);
long sum3(long);
long sum5(long);
long sum10(long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (long i = -1000; i < 1000; i++) {
        ASSERT(sum1(i) == i);
        ASSERT(sum3(i) == 3 * i + 3);
        ASSERT(sum5(i) == 5 * i + 10);
        ASSERT(sum10(i) == 10 * i + 45);
    }
    return EXIT_SUCCESS;
}
