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

struct Structure1 {
    int value;
};

struct Structure1 zero() {
    return (struct Structure1){0};
}

struct Structure1 one() {
    return (struct Structure1){1};
}

int test(int x) {
    struct Structure1 s1 = {.value = x};
    *(one(), &s1) = zero();
    return s1.value == 0;
}
