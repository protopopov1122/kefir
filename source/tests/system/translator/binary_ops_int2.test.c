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

unsigned int sum(unsigned int, unsigned int);
unsigned int sub(unsigned int, unsigned int);
unsigned int mul(unsigned int, unsigned int);
unsigned int divide(unsigned int, unsigned int);
unsigned int modulo(unsigned int, unsigned int);
unsigned int shl(unsigned int, unsigned int);
unsigned int shr(unsigned int, unsigned int);
unsigned int band(unsigned int, unsigned int);
unsigned int bor(unsigned int, unsigned int);
unsigned int bxor(unsigned int, unsigned int);
unsigned int less(unsigned int, unsigned int);
unsigned int lesseq(unsigned int, unsigned int);
unsigned int greater(unsigned int, unsigned int);
unsigned int greatereq(unsigned int, unsigned int);
unsigned int equals(unsigned int, unsigned int);
unsigned int noteq(unsigned int, unsigned int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (unsigned int i = 0; i < 200; i++) {
        for (unsigned int j = 0; j < 200; j++) {
            ASSERT(sum(i, j) == i + j);
            ASSERT(sub(i, j) == i - j);
            ASSERT(mul(i, j) == i * j);
            if (j != 0) {
                ASSERT(divide(i, j) == i / j);
                ASSERT(modulo(i, j) == i % j);
            }

            if (j < (sizeof(int) * CHAR_BIT)) {
                ASSERT(shl(i, j) == i << j);
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
