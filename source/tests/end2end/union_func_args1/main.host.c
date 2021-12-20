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
        struct Struct1 s = {.c1 = i, .c2 = ~i, .s1 = -i, .i1 = i / 2, .i2 = i + 1, .i3 = i ^ 0xffe, .i4 = i & 0xffe};

        assert(struct1_calculate(s) == STRUCT1_CALCULATE(&s));
    }
    return EXIT_SUCCESS;
}
