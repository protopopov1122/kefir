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

long ldouble_to_long(long double);
long double ldouble_from_long(long);
long double ldouble_from_ulong(unsigned long);
long double ldouble_from_float(float);
long double ldouble_from_double(double);
float ldouble_to_float(long double);
double ldouble_to_double(long double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long double x = -100.0; x < 100.0; x += 0.1) {
        ASSERT(ldouble_to_long(x) == (long) x);
        ASSERT(FLOAT_EQUALS(ldouble_to_float(x), (float) x, FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(ldouble_to_double(x), (double) x, DOUBLE_EPSILON));
    }

    for (long l = -1000; l < 1000; l++) {
        ASSERT(LONG_DOUBLE_EQUALS(ldouble_from_long(l), (long double) l, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(ldouble_from_ulong((unsigned long) l), (long double) (unsigned long) l,
                                  LONG_DOUBLE_EPSILON));
    }

    for (float f = -100.0f; f < 100.0f; f += 0.02f) {
        ASSERT(LONG_DOUBLE_EQUALS(ldouble_from_float(f), (long double) f, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(ldouble_from_double(f), (long double) f, LONG_DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
