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
#include <assert.h>
#include <math.h>
#include "./definitions.h"

static unsigned long dim = 0;
unsigned long matrix_dim() {
    return dim * 2;
}

int main() {
    assert(MATRIX_ALIGN == _Alignof(double[matrix_dim()][matrix_dim()]));
    for (unsigned long i = 0; i < 100; i++) {
        dim = i;
        assert(sizeof(int[i]) == int_vla_sizeof(i));
        assert(sizeof(double[i * 2][i]) == double_matrix_sizeof(i));
        assert(_Alignof(short[i][i << 2][i / 2]) == short_cube_alignment(i, i << 2, i / 2));
    }
    return EXIT_SUCCESS;
}
