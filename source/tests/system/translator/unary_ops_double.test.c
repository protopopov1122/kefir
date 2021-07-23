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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

double plus(double);
double negate(double);
int logical_negate(double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double i = -10.0f; i < 10.0f; i += 0.1f) {
        ASSERT(DOUBLE_EQUALS(plus(i), i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(negate(i), -i, DOUBLE_EPSILON));
        ASSERT(logical_negate(i) == !i);
    }
    return EXIT_SUCCESS;
}
