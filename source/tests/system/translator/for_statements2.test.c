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

void transpose_matrix(long long *, unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    long long MATRIX[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
    transpose_matrix(MATRIX, 4);
    ASSERT(MATRIX[0] == 0x0);
    ASSERT(MATRIX[1] == 0x4);
    ASSERT(MATRIX[2] == 0x8);
    ASSERT(MATRIX[3] == 0xc);
    ASSERT(MATRIX[4] == 0x1);
    ASSERT(MATRIX[5] == 0x5);
    ASSERT(MATRIX[6] == 0x9);
    ASSERT(MATRIX[7] == 0xd);
    ASSERT(MATRIX[8] == 0x2);
    ASSERT(MATRIX[9] == 0x6);
    ASSERT(MATRIX[10] == 0xa);
    ASSERT(MATRIX[11] == 0xe);
    ASSERT(MATRIX[12] == 0x3);
    ASSERT(MATRIX[13] == 0x7);
    ASSERT(MATRIX[14] == 0xb);
    ASSERT(MATRIX[15] == 0xf);

    long long MATRIX2[] = {0x0, 0x1, 0x2, 0x3};
    transpose_matrix(MATRIX2, 2);
    ASSERT(MATRIX2[0] == 0x0);
    ASSERT(MATRIX2[1] == 0x2);
    ASSERT(MATRIX2[2] == 0x1);
    ASSERT(MATRIX2[3] == 0x3);
    return EXIT_SUCCESS;
}
