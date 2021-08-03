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

long *preinc(long **);
long *postinc(long **);
long *predec(long **);
long *postdec(long **);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
#define LEN 128
    long array[LEN];
    long *a = &array[0];
    long *b = &array[0];
    for (int i = 0; i < LEN; i++) {
        ASSERT(a == &array[i]);
        ASSERT(b == &array[i]);
        ASSERT(preinc(&a) == &array[i + 1]);
        ASSERT(postinc(&b) == &array[i]);
        ASSERT(a == &array[i + 1]);
        ASSERT(b == &array[i + 1]);
    }
    a = &array[LEN];
    b = &array[LEN];
    for (int i = LEN; i > 0; i--) {
        ASSERT(a == &array[i]);
        ASSERT(b == &array[i]);
        ASSERT(predec(&a) == &array[i - 1]);
        ASSERT(postdec(&b) == &array[i]);
        ASSERT(a == &array[i - 1]);
        ASSERT(b == &array[i - 1]);
    }
    return EXIT_SUCCESS;
}
