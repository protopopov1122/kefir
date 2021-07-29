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

unsigned int preinc(unsigned int *);
unsigned int postinc(unsigned int *);
unsigned int predec(unsigned int *);
unsigned int postdec(unsigned *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (unsigned int i = 0, a = i, b = i; i < 200; i++) {
        ASSERT(a == i);
        ASSERT(b == i);
        ASSERT(preinc(&a) == i + 1);
        ASSERT(postinc(&b) == i);
        ASSERT(a == i + 1);
        ASSERT(b == i + 1);
    }
    for (unsigned int i = 200, a = i, b = i; i > 0; i--) {
        ASSERT(a == i);
        ASSERT(b == i);
        ASSERT(predec(&a) == i - 1);
        ASSERT(postdec(&b) == i);
        ASSERT(a == i - 1);
        ASSERT(b == i - 1);
    }
    return EXIT_SUCCESS;
}
