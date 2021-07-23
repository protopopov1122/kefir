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

long long sum_matrix(long long *, unsigned int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long long i = -1000; i < 1000; i++) {
        long long matrix[] = {1 + i, 2, 3, 4, 5, 6 * i, 7, 8, 9 - (~i)};
        ASSERT(sum_matrix(matrix, 3) == 39 + i - (~i) + 6 * i);
    }
    return EXIT_SUCCESS;
}
