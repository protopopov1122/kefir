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

#include "./definitions.h"

void test_vla(int iterations, int maxlen) {
    int n = 1;
    int array[maxlen];
preloop1:
    if (n != iterations) {
        goto loop1;
    } else {
        long array2[maxlen];
        return;
    }
loop1:;
    int length = n % maxlen + 1;
    char buffer[length];
    for (int i = 0; i < length; i++) {
        buffer[i] = (char) i;
    }

    if (n == iterations) {
        goto exit_loop;
    } else {
        n++;
    }

    goto loop1;

exit_loop:
    goto preloop1;
}
