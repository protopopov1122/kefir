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

double compound_test(double, double, double, double, double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double base = -2.0; base < 2.0; base += 0.1) {
        for (double add = -4.0; add < 4.0; add += 0.1) {
            for (double multiply = -100; multiply < 100; multiply += 25) {
                ASSERT(DOUBLE_EQUALS((base + add) / (1 / base) * multiply - add,
                                     compound_test(base, add, 1 / base, multiply, add), DOUBLE_EPSILON));
            }
        }
    }
    return EXIT_SUCCESS;
}
