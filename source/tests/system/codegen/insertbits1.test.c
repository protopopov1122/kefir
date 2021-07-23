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

unsigned long insertbits(char, char, char, char, char, char, char, char);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (unsigned int i = 0; i < 0x100; i++) {
        ASSERT(insertbits((i >> 7) & 1, (i >> 6) & 1, (i >> 5) & 1, (i >> 4) & 1, (i >> 3) & 1, (i >> 2) & 1,
                          (i >> 1) & 1, (i >> 0) & 1) == i);
    }
    return EXIT_SUCCESS;
}
