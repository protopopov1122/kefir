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
#include <assert.h>
#include "./definitions.h"

int main() {
    for (int i = -100; i < 100; i++) {
        struct Struct1 s = {.lng1 = i, .lng2 = i * 2, .chr1 = (char) i, .int1 = -i, .shr1 = i & 15, .shr2 = i ^ 1023};

        assert(struct1_calculate(s) == STRUCT1_CALCULATE(s));
    }
    assert(STRUCTURE.lng1 == 1);
    assert(STRUCTURE.lng2 == 2);
    assert(STRUCTURE.chr1 == 3);
    assert(STRUCTURE.int1 == 4);
    assert(STRUCTURE.int2 == 5);

    assert(STRUCTURE2.i1 == 3);
    assert(STRUCTURE2.i2 == 1023);
    assert(STRUCTURE2.i3 == -4);
    assert(STRUCTURE2.i4 == -501);
    assert(STRUCTURE2.i5 == 0);
    assert(STRUCTURE2.i6 == -5);
    assert(STRUCTURE2.i7 == 10);
    return EXIT_SUCCESS;
}
