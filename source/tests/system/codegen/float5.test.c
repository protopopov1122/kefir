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
#include <math.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

float fneg(float);
double dneg(double);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -1000.0f; i < 1000.0f; i += 0.001f) {
        ASSERT(FLOAT_EQUALS(fneg(i), -i, FLOAT_EPSILON));
        ASSERT(DOUBLE_EQUALS(dneg(i), -((double) i), DOUBLE_EPSILON));
    }
    return EXIT_SUCCESS;
}
