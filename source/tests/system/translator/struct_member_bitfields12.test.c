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
    char a : 8;
    short b : 9;
    int c : 6;
    char d : 1;
    char e : 1;
};

int sum(struct param *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (int i = -100; i <= 100; i++) {
        ASSERT(sum(&(struct param){.a = 54, .b = i, .c = -5, .d = 0, .e = -1}) ==
               (int) (54 + i - 5 - 1 + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){.a = 54, .b = i, .c = -5, .d = 0, .e = 0}) ==
               (int) (54 + i - 5 + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){.a = 54, .b = i, .c = -5, .d = -1, .e = 0}) ==
               (int) (54 + i - 5 - 1 + sizeof(struct param) + _Alignof(struct param)));

        ASSERT(sum(&(struct param){.a = 54, .b = i, .c = -5, .d = -1, .e = -1}) ==
               (int) (54 + i - 5 - 1 - 1 + sizeof(struct param) + _Alignof(struct param)));
    }
    return EXIT_SUCCESS;
}
