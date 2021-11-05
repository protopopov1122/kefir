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

static unsigned int DIM[3] = {0, 0, 0};
unsigned int dim1() {
    unsigned int val = DIM[0];
    DIM[0] = 0;
    return val;
}

unsigned int dim2() {
    unsigned int val = DIM[1];
    DIM[1] = 0;
    return val;
}

unsigned int dim3() {
    unsigned int val = DIM[2];
    DIM[2] = 0;
    return val;
}

int main() {
    assert(sizeof_float_cube() == 0);
    DIM[0] = 1;
    DIM[1] = 2;
    DIM[2] = 3;
    assert(sizeof_float_cube() == 6 * sizeof(float));
    assert(sizeof_float_cube() == 0);
    DIM[0] = 25;
    DIM[1] = 5;
    DIM[2] = 5;
    assert(sizeof_float_cube() == 625 * sizeof(float));
    assert(sizeof_float_cube() == 0);
    for (int i = 0; i < 20; i++) {
        DIM[0] = i;
        DIM[1] = i + 1;
        DIM[2] = i + 2;
        assert(sizeof_float_cube() == i * (i + 1) * (i + 2) * sizeof(float));
        assert(sizeof_float_cube() == 0);
    }
    return EXIT_SUCCESS;
}
