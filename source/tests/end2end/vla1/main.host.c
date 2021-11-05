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

static unsigned int ARR[2] = {0, 0};

void callback1(unsigned int val) {
    ARR[0] = val;
}

void callback2(unsigned int val) {
    ARR[1] = val;
}

int main() {
    assert(int_at(0, 0) == 0);
    assert(int_at(1, 0) == 1);
    assert(ARR[0] == 1);
    assert(ARR[1] == 0);
    assert(int_at(1, 1) == 0);
    assert(ARR[0] == 1);
    assert(ARR[1] == 1);
    for (unsigned int i = 0; i < 256; i++) {
        assert(int_at(256, i) == i + 1);
        assert(ARR[0] == 256);
        assert(ARR[1] == i);
    }
    return EXIT_SUCCESS;
}
