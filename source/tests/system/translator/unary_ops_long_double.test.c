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

long double plus(long double);
long double negate(long double);
int logical_negate(long double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long double i = -10.0f; i < 10.0f; i += 0.1f) {
        ASSERT(LONG_DOUBLE_EQUALS(plus(i), i, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(negate(i), -i, LONG_DOUBLE_EPSILON));
        ASSERT(logical_negate(i) == !i);
    }
    ASSERT(logical_negate(0.0l));
    ASSERT(!logical_negate(1.0l));
    return EXIT_SUCCESS;
}
