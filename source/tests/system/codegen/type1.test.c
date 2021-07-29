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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

bool trunc1(long);
char extend8(long);
short extend16(long);
int extend32(long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        ASSERT(trunc1(i) || i == 0);
    }
    for (int i = -1000; i < 1000; i++) {
        ASSERT(extend8(i) == ((char) i));
    }
    for (int i = -100000; i < 100000; i++) {
        ASSERT(extend16(i) == ((short) i));
        ASSERT(extend32(i) == i);
        const long val = 1000000l * i;
        ASSERT(extend32(val) == ((int) val));
    }
    return EXIT_SUCCESS;
}
