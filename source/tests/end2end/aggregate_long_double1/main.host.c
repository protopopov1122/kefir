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

static long double LD;
static struct Param1 getLD() {
    return (struct Param1){LD};
}

int main() {
    for (long double x = -100.0; x < 100.0; x += 0.01) {
        struct Param1 p = ldneg((struct Param1){x});
        assert(fabsl(x + p.value) < 1e-5);
        p = ldsum((struct Param2){x, 1.0});
        assert(fabsl(x + 1 - p.value) < 1e-5);

        assert(fabsl(ldvsum(0, x).value) < 1e-5);
        assert(fabsl(ldvsum(1, x).value - x) < 1e-5);
        assert(fabsl(ldvsum(2, x, x).value - 2 * x) < 1e-5);
        assert(fabsl(ldvsum(3, x, x, 1.0l).value - (2 * x + 1.0)) < 1e-5);

        assert(fabsl(ldvsum2(0, (struct Param1){x}).value) < 1e-5);
        assert(fabsl(ldvsum2(1, (struct Param1){x}).value - x) < 1e-5);
        assert(fabsl(ldvsum2(2, (struct Param1){x}, (struct Param1){x}).value - 2 * x) < 1e-5);
        assert(fabsl(ldvsum2(3, (struct Param1){x}, (struct Param1){x}, (struct Param1){1}).value - (2 * x + 1.0)) <
               1e-5);

        LD = x;
        assert(fabsl(ldunwrap(getLD) - x) < 1e-5);
    }
    return EXIT_SUCCESS;
}
