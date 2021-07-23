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

unsigned long extractbits(unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    ASSERT(extractbits(0xcafebabeu) == 0xe + 0xb + 0xa + 0xb + 0xe + 0xf + 0xa + 0xc);
    for (unsigned int i = 0; i <= 0xf; i++) {
        for (unsigned int j = 0; j <= 0xf; j++) {
            for (unsigned int k = 0; k <= 0xf; k++) {
                for (unsigned int a = 0; a <= 0xf; a++) {
                    ASSERT(extractbits((i << 24) | (j << 16) | (k << 8) | a) == i + j + k + a);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
