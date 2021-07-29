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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

const char *string1();
const char *string2();
const char *string3();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(strcmp(string1(), "Hello, world!") == 0);
    ASSERT(strcmp(string2(), "\n\n\t\tHey there\'\"!\v\n") == 0);
    ASSERT(string3()[0] == '\0');
    ASSERT(string3()[1] == '\0');
    ASSERT(string3()[2] == '\0');
    ASSERT(string3()[3] == '\0');
    ASSERT(strcmp(string1(), "Hello, world!") == 0);
    ASSERT(string3()[0] == '\0');
    ASSERT(string3()[1] == '\0');
    ASSERT(string3()[2] == '\0');
    ASSERT(string3()[3] == '\0');
    ASSERT(strcmp(string2(), "\n\n\t\tHey there\'\"!\v\n") == 0);
    return EXIT_SUCCESS;
}
