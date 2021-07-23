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
    long field1 : 15;
    short field2 : 11;
    int : 9;
    int field3 : 17;
    long field4 : 20;
    char : 0;
    long field5 : 33;
};

void modify(struct param *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -200; i < 200; i++) {
        for (int j = -100; j < 100; j++) {
            if (j == 0) {
                continue;
            }
            struct param val = {
                .field1 = 100 + i, .field2 = 321 + i, .field3 = 786 + i, .field4 = 912 + i, .field5 = 542018 + i};
            modify(&val, j);
            ASSERT(val.field1 == (100 + i) / j);
            ASSERT(val.field2 == (321 + i) / j);
            ASSERT(val.field3 == (786 + i) / j);
            ASSERT(val.field4 == (912 + i) / j);
            ASSERT(val.field5 == (542018 + i) / j);
        }
    }

    return EXIT_SUCCESS;
}
