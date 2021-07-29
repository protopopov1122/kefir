/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

bool fequals(float, float);
bool fgreater(float, float);
bool flesser(float, float);
bool dequals(double, double);
bool dgreater(double, double);
bool dlesser(double, double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -100.0f; i < 100.0f; i += 0.001f) {
        ASSERT(fequals(i, i));
        ASSERT(!fequals(i, i * 2));
        ASSERT(!fequals(i, i + 1));
        ASSERT(!fequals(i, i + FLOAT_EPSILON));

        ASSERT(fgreater(i + FLOAT_EPSILON, i));
        ASSERT(fgreater(fabs(i) * 2, fabs(i)));
        ASSERT(!fgreater(i, i));
        ASSERT(!fgreater(i - FLOAT_EPSILON, i));

        ASSERT(!flesser(i + FLOAT_EPSILON, i));
        ASSERT(!flesser(fabs(i) * 2, fabs(i)));
        ASSERT(!flesser(i, i));
        ASSERT(flesser(i - FLOAT_EPSILON, i));
        ASSERT(flesser(fabs(i) * 2, fabs(i) * 10));
    }
    for (double i = -100.0; i < 100.0; i += 0.001) {
        ASSERT(dequals(i, i));
        ASSERT(!dequals(i, i * 2));
        ASSERT(!dequals(i, i + 1));
        ASSERT(!dequals(i, i + DOUBLE_EPSILON));

        ASSERT(dgreater(i + DOUBLE_EPSILON, i));
        ASSERT(dgreater(fabs(i) * 2, fabs(i)));
        ASSERT(!dgreater(i, i));
        ASSERT(!dgreater(i - DOUBLE_EPSILON, i));

        ASSERT(!dlesser(i + DOUBLE_EPSILON, i));
        ASSERT(!dlesser(fabs(i) * 2, fabs(i)));
        ASSERT(!dlesser(i, i));
        ASSERT(dlesser(i - DOUBLE_EPSILON, i));
        ASSERT(dlesser(fabs(i) * 2, fabs(i) * 10));
    }
    return EXIT_SUCCESS;
}
