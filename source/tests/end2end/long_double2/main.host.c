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

static long double GENERATOR_VALUE = 0.0L;

static long double generator() {
    return GENERATOR_VALUE;
}

int main() {
    for (long double x = -20.0l; x < 20.0l; x += 0.02l) {
        GENERATOR_VALUE = x;
        long double y = increment(generator);
        assert(fabsl(LAST_VALUE - x) < 1e-6);
        assert(fabsl(y - (x + 1)) < 1e-6);
    }
    return EXIT_SUCCESS;
}
