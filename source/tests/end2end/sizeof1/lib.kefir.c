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

#include "./definitions.h"

extern const unsigned long MATRIX_ALIGN = _Alignof(double[matrix_dim()][matrix_dim()]);

unsigned long int_vla_sizeof(unsigned long length) {
    return sizeof(int[length]);
}

unsigned long double_matrix_sizeof(unsigned long length) {
    typedef double matrix_row_t[matrix_dim()];
    return sizeof(matrix_row_t[length]);
}

unsigned long short_cube_alignment(unsigned long x, unsigned long y, unsigned long z) {
    return _Alignof(short[x][y][z]);
}
