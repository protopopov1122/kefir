/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long floatToLong(float);
long doubleToLong(double);
float longToFloat(long);
double longToDouble(long);
double floatToDouble(float);
float doubleToFloat(double);
float ulongToFloat(unsigned long);
double ulongToDouble(unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -1000.0f; i < 1000.0f; i += 0.01) {
        ASSERT(floatToLong(i) == ((long) i));
        ASSERT(doubleToLong((double) i) == ((long) i));
        ASSERT(DOUBLE_EQUALS(floatToDouble(i), ((double) i), DOUBLE_EPSILON));
        ASSERT(FLOAT_EQUALS(doubleToFloat((double) i), i, FLOAT_EPSILON));
    }
    for (long i = -1000; i < 1000; i++) {
        ASSERT(FLOAT_EQUALS(longToFloat((long) i), (float) i, FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(longToDouble((long) i), (double) i, DOUBLE_EPSILON));
    }
    for (unsigned long i = 0; i < 10000; i++) {
        ASSERT(FLOAT_EQUALS(ulongToFloat(i), (float) i, FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(ulongToDouble(i), (double) i, DOUBLE_EPSILON));
        ASSERT(FLOAT_EQUALS(ulongToFloat(ULONG_MAX - i), (float) (ULONG_MAX - i), FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(ulongToDouble(ULONG_MAX - i), (double) (ULONG_MAX - i), DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
