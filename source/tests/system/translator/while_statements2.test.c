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

long long parseint(const char[]);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(parseint(NULL) == 0);
    ASSERT(parseint("") == 0);
    ASSERT(parseint("-0") == 0);
    ASSERT(parseint("1") == 1);
    ASSERT(parseint("-1") == -1);
    ASSERT(parseint("-1aaaaab") == -1);
    ASSERT(parseint("-.2aaaaab") == 0);
    ASSERT(parseint("80015730") == 80015730);
    ASSERT(parseint("8001 5730") == 8001);
    ASSERT(parseint("-1234") == -1234);
    ASSERT(parseint("10-2") == 10);
    ASSERT(parseint("905648") == 905648);
    ASSERT(parseint("-1112223348168a7") == -1112223348168);
    return EXIT_SUCCESS;
}
