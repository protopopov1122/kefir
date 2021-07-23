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
    int field1 : 10;
    int field2 : 7;
    short field3 : 9;
    char : 0;
    char : 6;
    long field4 : 19;
    int field5 : 15;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -10; i < 10; i++) {
        for (int j = -5; j < 5; j++) {
            struct param val = {.field1 = i * 2, .field2 = i, .field3 = i, .field4 = i * 10, .field5 = i * 7};
            modify(&val, j);
            ASSERT(val.field1 == i * j * 2);
            ASSERT(val.field2 == i * j);
            ASSERT(val.field3 == i * j);
            ASSERT(val.field4 == i * j * 10);
            ASSERT(val.field5 == i * j * 7);
        }
    }

    return EXIT_SUCCESS;
}
