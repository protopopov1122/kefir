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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long field1 : 9;
    short field2 : 12;
    int field3 : 20;
    char : 0;
    int : 14;
    long field4 : 24;
    char : 6;
    char field5 : 7;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -10000; i < 10000; i += 50) {
        for (int j = -50; j < 50; j++) {
            struct param val = {.field1 = 110, .field2 = 1020, .field3 = i / 2, .field4 = i, .field5 = 0};
            modify(&val, j);
            ASSERT(val.field1 == 110 + j);
            ASSERT(val.field2 == 1020 + j);
            ASSERT(val.field3 == i / 2 + j);
            ASSERT(val.field4 == i + j);
            ASSERT(val.field5 == j);
        }
    }

    return EXIT_SUCCESS;
}
