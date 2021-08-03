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

int dummy_factorial(int);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int i = -100; i < 100; i++) {
        int fact = dummy_factorial(i);
        switch (i) {
            case 1:
                ASSERT(fact == 1);
                break;

            case 2:
                ASSERT(fact == 2);
                break;

            case 3:
                ASSERT(fact == 6);
                break;

            case 4:
                ASSERT(fact == 24);
                break;

            case 5:
                ASSERT(fact == 120);
                break;

            case 6:
                ASSERT(fact == 720);
                break;

            case 7:
                ASSERT(fact == 5040);
                break;

            case 8:
                ASSERT(fact == 40320);
                break;

            case 9:
                ASSERT(fact == 362880);
                break;

            default:
                if (i >= 10) {
                    ASSERT(fact == 3628800);
                } else {
                    ASSERT(fact == -1);
                }
                break;
        }
    }
    return EXIT_SUCCESS;
}
