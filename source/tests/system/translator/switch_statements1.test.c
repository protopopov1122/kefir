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

int char_to_int(char);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (char c = CHAR_MIN; c < CHAR_MAX; c++) {
        if (c >= '0' && c <= '9') {
            ASSERT(char_to_int(c) == c - '0');
        } else {
            ASSERT(char_to_int(c) == -1);
        }
    }
    return EXIT_SUCCESS;
}
