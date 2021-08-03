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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

unsigned long fibonacci(unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(fibonacci(0) == 0);
    ASSERT(fibonacci(1) == 1);
    ASSERT(fibonacci(2) == 1);
    ASSERT(fibonacci(3) == 2);
    ASSERT(fibonacci(4) == 3);
    ASSERT(fibonacci(5) == 5);
    ASSERT(fibonacci(6) == 8);
    ASSERT(fibonacci(7) == 13);
    ASSERT(fibonacci(8) == 21);
    ASSERT(fibonacci(9) == 34);
    ASSERT(fibonacci(10) == 55);
    ASSERT(fibonacci(11) == 89);
    ASSERT(fibonacci(12) == 144);
    return EXIT_SUCCESS;
}
