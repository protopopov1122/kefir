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

float preinc(float *);
float postinc(float *);
float predec(float *);
float postdec(float *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (float i = -10.0f; i < 10.0; i += 0.1f) {
        float a = i, b = i;
        ASSERT(FLOAT_EQUALS(a, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(preinc(&a), i + 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(postinc(&b), i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(a, i + 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i + 1, FLOAT_EPSILON));
    }
    for (float i = -10.0f; i < 10.0; i += 0.1f) {
        float a = i, b = i;
        ASSERT(FLOAT_EQUALS(a, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(predec(&a), i - 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(postdec(&b), i, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(a, i - 1, FLOAT_EPSILON));
        ASSERT(FLOAT_EQUALS(b, i - 1, FLOAT_EPSILON));
    }
    return EXIT_SUCCESS;
}
