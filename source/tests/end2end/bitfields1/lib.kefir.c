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

#include "./definitions.h"

long struct1_calculate(struct Struct1 s) {
    return STRUCT1_CALCULATE(s);
}

struct Struct1 STRUCTURE = {.lng1 = 1, .lng2 = 2, .chr1 = 3, .int1 = 4, .int2 = 5};

struct Struct2 STRUCTURE2 = {.i1 = 3, .i2 = 1023, .i3 = -4, .i4 = -501, .i5 = 0, .i6 = -5, .i7 = 10};

void fill(struct Struct2 *s) {
    *s = (struct Struct2){.i1 = 1, .i2 = 2, .i3 = 3, .i4 = 4, .i5 = 5, .i6 = 6, .i7 = 7};
}
