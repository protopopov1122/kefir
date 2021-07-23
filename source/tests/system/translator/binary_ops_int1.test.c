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

int sum(int, int);
int sub(int, int);
int mul(int, int);
int divide(int, int);
int modulo(int, int);
int shl(int, int);
int shr(int, int);
int band(int, int);
int bor(int, int);
int bxor(int, int);
int less(int, int);
int lesseq(int, int);
int greater(int, int);
int greatereq(int, int);
int equals(int, int);
int noteq(int, int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        for (int j = -100; j < 100; j++) {
            ASSERT(sum(i, j) == i + j);
            ASSERT(sub(i, j) == i - j);
            ASSERT(mul(i, j) == i * j);
            if (j != 0) {
                ASSERT(divide(i, j) == i / j);
                ASSERT(modulo(i, j) == i % j);
            }

            if (j >= 0 && j < (int) (sizeof(int) * CHAR_BIT) - 8) {
                if (i >= 0) {
                    ASSERT(shl(i, j) == i << j);
                }
                ASSERT(shr(i, j) == i >> j);
            }
            ASSERT(band(i, j) == (i & j));
            ASSERT(bor(i, j) == (i | j));
            ASSERT(bxor(i, j) == (i ^ j));
            ASSERT(less(i, j) == (i < j));
            ASSERT(lesseq(i, j) == (i <= j));
            ASSERT(greater(i, j) == (i > j));
            ASSERT(greatereq(i, j) == (i >= j));
            ASSERT(equals(i, j) == (i == j));
            ASSERT(noteq(i, j) == (i != j));
        }
    }
    return EXIT_SUCCESS;
}
