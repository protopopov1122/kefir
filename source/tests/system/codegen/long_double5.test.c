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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

kefir_bool_t ldouble_equals(long double, long double);
kefir_bool_t ldouble_greater(long double, long double);
kefir_bool_t ldouble_lesser(long double, long double);
int ldouble_trunc1(long double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long double x = -100.0; x < 100.0; x += 0.1) {
        ASSERT(ldouble_equals(x, x));
        ASSERT(!ldouble_equals(x, x + 1));
        ASSERT(!ldouble_equals(x, x * 2));

        ASSERT(!ldouble_greater(x, x));
        ASSERT(ldouble_greater(x + 1, x));
        if (x >= 0.0) {
            ASSERT(ldouble_greater(x, x / 10));
        } else {
            ASSERT(!ldouble_greater(x, x / 10));
        }

        ASSERT(!ldouble_lesser(x, x));
        ASSERT(ldouble_lesser(x, x + 1));
        if (x >= 0.0) {
            ASSERT(!ldouble_lesser(x, x / 10));
        } else {
            ASSERT(ldouble_lesser(x, x / 10));
        }

        ASSERT(ldouble_trunc1(x) == (x && 1));
    }
    return EXIT_SUCCESS;
}
