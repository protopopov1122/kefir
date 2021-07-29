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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    long a : 33;
    char b : 8;
    char : 0;
    short c : 7;
    long d : 17;
    int e : 22;
    char : 5;
    long f : 39;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -2000; i < 2000; i++) {
        for (int j = -1000; j < 1000; j += 250) {
            ASSERT(sum(&(struct param){.a = 2 * -i, .b = 2, .c = 10, .d = -60, .e = i, .f = j}) ==
                   (int) (2 * -i + 2 + 10 - 60 + i + j + sizeof(struct param) + _Alignof(struct param)));
        }
    }
    return EXIT_SUCCESS;
}
