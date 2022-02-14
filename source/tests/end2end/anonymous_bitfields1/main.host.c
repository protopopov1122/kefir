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
    assert(S1.i = 40);
    assert(S1.j = 30);
    assert(S1.k = 20);
    assert(S1.m = 10);
    for (int i = -10; i < 10; i++) {
        struct Structure1 s = fills(i);
        assert(s.i == i + 3);
        assert(s.j == i + 2);
        assert(s.k == i + 1);
        assert(s.m == i);
    }
    return EXIT_SUCCESS;
}
