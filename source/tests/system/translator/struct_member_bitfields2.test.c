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
    char f1 : 5;
    char f2 : 5;
    char : 0;
    char f3 : 2;
    char : 3;
    int f4 : 10;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -8; i < 8; i++) {
        for (int j = -8; j < 8; j++) {
            for (int k = -100; k < 100; k++) {
                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = -2, .f4 = k}) ==
                       (int) (i + j + k - 2 + sizeof(struct param) + _Alignof(struct param)));

                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = -1, .f4 = k}) ==
                       (int) (i + j + k - 1 + sizeof(struct param) + _Alignof(struct param)));

                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = 0, .f4 = k}) ==
                       (int) (i + j + k + sizeof(struct param) + _Alignof(struct param)));

                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = 1, .f4 = k}) ==
                       (int) (i + j + k + 1 + sizeof(struct param) + _Alignof(struct param)));
            }
        }
    }
    return EXIT_SUCCESS;
}
