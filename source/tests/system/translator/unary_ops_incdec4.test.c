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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double preinc(double *);
double postinc(double *);
double predec(double *);
double postdec(double *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double i = -10.0f; i < 10.0; i += 0.1f) {
        double a = i, b = i;
        ASSERT(DOUBLE_EQUALS(a, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(preinc(&a), i + 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(postinc(&b), i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(a, i + 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i + 1, DOUBLE_EPSILON));
    }
    for (double i = -10.0f; i < 10.0; i += 0.1f) {
        double a = i, b = i;
        ASSERT(DOUBLE_EQUALS(a, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(predec(&a), i - 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(postdec(&b), i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(a, i - 1, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(b, i - 1, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
