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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct structure {
    const char string[32];
    unsigned int length;
    _Alignas(16) unsigned long padding;
    void *ptr;
    union {
        double fp64;
        float fp32[2];
    } floats[4];
};

struct structure compound1();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    struct structure c1 = compound1();
    ASSERT(strcmp(c1.string, "Goodbye, world!") == 0);
    ASSERT(c1.length == strlen("Goodbye, world!"));
    ASSERT(c1.padding == 1010);
    ASSERT(strcmp(c1.ptr, "Goodbye, world!") == 0);
    ASSERT(DOUBLE_EQUALS(c1.floats[0].fp64, 4.0013, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(c1.floats[1].fp64, 19.88263, DOUBLE_EPSILON));
    ASSERT(FLOAT_EQUALS(c1.floats[2].fp32[0], 1.02f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(c1.floats[2].fp32[1], 2.04f, FLOAT_EPSILON));
    ASSERT(DOUBLE_EQUALS(c1.floats[3].fp64, 1e5, DOUBLE_EPSILON));
    return EXIT_SUCCESS;
}
