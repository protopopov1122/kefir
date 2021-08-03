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

double unwrap_double(double *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (double d = -100.0; d < 100.0; d += 0.01) {
        double res = unwrap_double(&d);
        if (d < 1.0) {
            ASSERT(DOUBLE_EQUALS(res, 0.0, DOUBLE_EPSILON));
        } else {
            ASSERT(DOUBLE_EQUALS(res, 1.0 / d, DOUBLE_EPSILON));
        }
    }
    ASSERT(DOUBLE_EQUALS(unwrap_double(NULL), -1.0, DOUBLE_EPSILON));
    return EXIT_SUCCESS;
}
