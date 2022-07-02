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

struct Structure1 {
    long a;
    int b[10];
    double c;
};

struct Structure1 X = {0};
struct Structure1 Y = {0};
_Thread_local struct Structure1 Z = {0};

int main() {
    assert(getx() == &X);
    assert(gety() == &Y);
    assert(getz() == &Z);
    return EXIT_SUCCESS;
}
