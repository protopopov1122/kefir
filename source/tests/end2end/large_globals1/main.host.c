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
    for (int i = 0; i < 10000; i++) {
        if (i == 1) {
            assert(Array[i].x == 1);
            assert(Array[i].y == 2);
            assert(Array[i].z == 3);
        } else if (i == 9998) {
            assert(Array[i].x == 1000);
            assert(Array[i].y == 2000);
            assert(Array[i].z == 3000);
        } else {
            assert(Array[i].x == 0);
            assert(Array[i].y == 0);
            assert(Array[i].z == 0);
        }
        assert(Array2[i].x == 0);
        assert(Array2[i].y == 0);
        assert(Array2[i].z == 0);
    }
    return EXIT_SUCCESS;
}
