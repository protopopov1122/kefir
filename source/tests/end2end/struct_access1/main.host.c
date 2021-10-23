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

int main() {
    struct S s = {0};
    assert(get_a(&s) == &s.a);
    assert(get_b(&s) == &s.b);
    assert(get_c(&s) == &s.c);
    assert(get_d(&s) == &s.d);
    assert(get_x(&s) == &s.d.x);
    assert(get_y(&s) == &s.d.y);
    assert(get_z(&s) == &s.d.z);
    assert(get_Z(&s) == &s.d.Z);
    return EXIT_SUCCESS;
}
