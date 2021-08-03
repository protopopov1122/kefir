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

struct circle_res {
    double length;
    double area;
    double neg_radius;
};

struct circle_res circle(double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double i = 0.0; i < 1000.0; i += 0.01) {
        struct circle_res res = circle(i);
        ASSERT(DOUBLE_EQUALS(res.length, 2.0 * 3.14159 * i, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(res.area, 3.14159 * ((double) (i * i)) / 2.0, DOUBLE_EPSILON));
        ASSERT(DOUBLE_EQUALS(res.neg_radius, -i, DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
