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
    for (int i = -100; i < 100; i++) {
        assert(zeros(1, 1, i) == i);
        assert(zeros(1, 0, 1, i) == 1 + i);
        assert(zeros(1, 0, 0, 0, 1, i) == 3 + i);
        assert(zeros(1, 0, 0, 0, 0, 0, 1, i) == 5 + i);
        assert(zeros(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, i) == 8 + i);
        assert(zeros(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, i) == 11 + i);
        assert(zeros(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, i) == 13 + i);
        assert(zeros(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, i) == 17 + i);
    }
    return EXIT_SUCCESS;
}
