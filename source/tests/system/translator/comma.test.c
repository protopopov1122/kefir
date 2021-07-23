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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

int callback(void *ptr) {
    int *i = (void *) ptr;
    return (*i)++;
}

int comma1(int (*)(void *), void *);
int comma2(int (*)(void *), void *);
int comma5(int (*)(void *), void *);
int comma10(int (*)(void *), void *);
int comma20(int (*)(void *), void *);
int comma50(int (*)(void *), void *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    int i = 0;
    ASSERT(comma1(callback, &i) == 0);
    ASSERT(i == 1);

    i = 0;
    ASSERT(comma2(callback, &i) == 1);
    ASSERT(i == 2);

    i = 0;
    ASSERT(comma5(callback, &i) == 4);
    ASSERT(i == 5);

    i = 0;
    ASSERT(comma10(callback, &i) == 9);
    ASSERT(i == 10);

    i = 0;
    ASSERT(comma20(callback, &i) == 19);
    ASSERT(i == 20);

    i = 0;
    ASSERT(comma50(callback, &i) == 49);
    ASSERT(i == 50);
    return EXIT_SUCCESS;
}
