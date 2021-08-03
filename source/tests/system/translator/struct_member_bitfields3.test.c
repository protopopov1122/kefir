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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct param {
    char f1 : 3;
    char f2 : 3;
    unsigned short f3 : 1;
    int f4 : 19;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -4; i < 3; i++) {
        for (int j = -4; j < 3; j++) {
            for (int k = -256; k < 256; k++) {
                int kval = k < 0 ? -(-k << 3) : (k << 3);
                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = 0, .f4 = kval}) ==
                       (int) (i + j + kval + sizeof(struct param) + _Alignof(struct param)));

                ASSERT(sum(&(struct param){.f1 = i, .f2 = j, .f3 = 1, .f4 = kval}) ==
                       (int) (i + j + kval + 1 + sizeof(struct param) + _Alignof(struct param)));
            }
        }
    }
    return EXIT_SUCCESS;
}
