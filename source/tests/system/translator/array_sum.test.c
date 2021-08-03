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

float array_sum(float *, long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    float FLOATS[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    unsigned int LEN = sizeof(FLOATS) / sizeof(FLOATS[0]);

    ASSERT(FLOAT_EQUALS(array_sum(FLOATS, 0), 0.0f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array_sum(FLOATS, 1), 1.0f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array_sum(FLOATS + 8, 1), 9.0f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array_sum(FLOATS, LEN), 45.0f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array_sum(FLOATS + 1, LEN - 1), 44.0f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array_sum(FLOATS + 3, LEN - 3), 39.0f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array_sum(FLOATS + 5, LEN - 5), 30.0f, FLOAT_EPSILON));
    return EXIT_SUCCESS;
}
