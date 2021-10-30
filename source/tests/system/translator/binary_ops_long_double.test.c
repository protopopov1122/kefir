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

long double sum(long double, long double);
long double sub(long double, long double);
long double mul(long double, long double);
long double divide(long double, long double);
int less(long double, long double);
int lesseq(long double, long double);
int greater(long double, long double);
int greatereq(long double, long double);
int equals(long double, long double);
int noteq(long double, long double);
int logical_and(long double, long double);
int logical_or(long double, long double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long double i = -10.0f; i < 10.0f; i += 0.1f) {
        for (long double j = -10.0f; j < 10.0f; j += 0.1f) {
            ASSERT(LONG_DOUBLE_EQUALS(sum(i, j), i + j, LONG_DOUBLE_EPSILON));
            ASSERT(LONG_DOUBLE_EQUALS(sub(i, j), i - j, LONG_DOUBLE_EPSILON));
            ASSERT(LONG_DOUBLE_EQUALS(mul(i, j), i * j, LONG_DOUBLE_EPSILON));
            ASSERT(LONG_DOUBLE_EQUALS(divide(i, j), i / j, LONG_DOUBLE_EPSILON));

            ASSERT(less(i, j) == (i < j));
            ASSERT(lesseq(i, j) == (i <= j));
            ASSERT(greater(i, j) == (i > j));
            ASSERT(greatereq(i, j) == (i >= j));
            ASSERT(equals(i, j) == (i == j));
            ASSERT(noteq(i, j) == (i != j));

            ASSERT(logical_and(i, j) == (i && j));
            ASSERT(logical_or(i, j) == (i || j));
        }
        ASSERT(lesseq(i, i - 1) == (i <= i - 1));
        ASSERT(lesseq(i, i + 1) == (i <= i + 1));
        ASSERT(greatereq(i, i - 1) == (i >= i - 1));
        ASSERT(greatereq(i, i + 1) == (i >= i + 1));
    }
    return EXIT_SUCCESS;
}
