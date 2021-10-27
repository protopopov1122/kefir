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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

long double sumldouble(int, ...);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(LONG_DOUBLE_EQUALS(sumldouble(0), 1.0, LONG_DOUBLE_EPSILON));
    ASSERT(LONG_DOUBLE_EQUALS(sumldouble(0, 1000.05l), 1.0, LONG_DOUBLE_EPSILON));
    ASSERT(LONG_DOUBLE_EQUALS(sumldouble(1, 0.0l), 1.0, LONG_DOUBLE_EPSILON));
    ASSERT(LONG_DOUBLE_EQUALS(sumldouble(1, 12.452l), 13.452, LONG_DOUBLE_EPSILON));
    ASSERT(LONG_DOUBLE_EQUALS(sumldouble(2, 0.0714e-6l, 100.2651e2l), 0.0714e-6l + 100.2651e2l + 1.0l,
                              LONG_DOUBLE_EPSILON));
    ASSERT(LONG_DOUBLE_EQUALS(sumldouble(4, 1.0l, 2.0l, 3.0l, 4.0l), 11.0l, LONG_DOUBLE_EPSILON));
    ASSERT(LONG_DOUBLE_EQUALS(
        sumldouble(10, 10.0001l, 11.0001l, 12.0001l, -100.0l, 0.0l, 0.0001e-2l, -5.1l, 10281.011l, -1.123l, 5820.0l),
        10.0001l + 11.0001l + 12.0001l + -100.0l + 0.0l + 0.0001e-2l + -5.1l + 10281.011l + -1.123l + 5820.0l + 1.0l,
        LONG_DOUBLE_EPSILON));
    return EXIT_SUCCESS;
}
