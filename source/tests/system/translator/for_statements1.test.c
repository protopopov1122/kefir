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

long long sumall(long long[]);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(sumall((long long[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == 45);
    ASSERT(sumall((long long[]){0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == 45);
    ASSERT(sumall((long long[]){1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 0}) == 1);
    ASSERT(sumall((long long[]){1000, 100, 900, 200, 0, 300}) == 2200);
    ASSERT(sumall((long long[]){0, 1000, 100, 900, 200, 0, 300}) == 2200);
    ASSERT(sumall((long long[]){0, 0, 1000, 100, 900, 200, 0, 300}) == 0);
    return EXIT_SUCCESS;
}
