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
#include "./definitions.h"

STRUCT1;
ENUM1;

void get_vals(struct Struct1, enum Enum1, int *, int *, int *);

int main() {
    int sz, algn, b_val;
    get_vals((struct Struct1){0}, A, &sz, &algn, &b_val);
    assert(sz == sizeof(struct Struct1));
    assert(algn == _Alignof(struct Struct1));
    assert(b_val == B);

    int array[256];
    for (int i = 1; i < (int) (sizeof(array) / sizeof(array[0])); i++) {
        assert(get_inc(i, array) == i + 1);
    }
    return EXIT_SUCCESS;
}
