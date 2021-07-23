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

float sum(float, float);
float sub(float, float);
float mul(float, float);
float divide(float, float);
int less(float, float);
int lesseq(float, float);
int greater(float, float);
int greatereq(float, float);
int equals(float, float);
int noteq(float, float);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -10.0f; i < 10.0f; i += 0.1f) {
        for (float j = -10.0f; j < 10.0f; j += 0.1f) {
            ASSERT(FLOAT_EQUALS(sum(i, j), i + j, FLOAT_EPSILON));
            ASSERT(FLOAT_EQUALS(sub(i, j), i - j, FLOAT_EPSILON));
            ASSERT(FLOAT_EQUALS(mul(i, j), i * j, FLOAT_EPSILON));
            ASSERT(FLOAT_EQUALS(divide(i, j), i / j, FLOAT_EPSILON));

            ASSERT(less(i, j) == (i < j));
            ASSERT(lesseq(i, j) == (i <= j));
            ASSERT(greater(i, j) == (i > j));
            ASSERT(greatereq(i, j) == (i >= j));
            ASSERT(equals(i, j) == (i == j));
            ASSERT(noteq(i, j) == (i != j));
        }
    }
    return EXIT_SUCCESS;
}
