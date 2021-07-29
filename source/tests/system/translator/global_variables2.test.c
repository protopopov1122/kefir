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

char *get_character1();
unsigned *get_integer1();
long *get_long1();
float *get_float1();
double *get_double1();
char *get_str1();
char **get_str2();
char **get_str3();
unsigned **get_int1ptr();
void **get_fnptr();
void **get_null_ptr();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(*get_character1() == 'C');
    ASSERT(*get_integer1() == 0xfefea6);
    ASSERT(*get_long1() == -1234543);
    ASSERT(FLOAT_EQUALS(*get_float1(), 9.57463f, FLOAT_EPSILON));
    ASSERT(DOUBLE_EQUALS(*get_double1(), 1.564e14, DOUBLE_EPSILON));
    ASSERT(strcmp(get_str1(), "Ping-pong-ping-pong") == 0);
    ASSERT(strcmp(*get_str2(), "   ....\t\t\t\\\n\n\n...TEST\n ") == 0);
    ASSERT(*get_str3() == get_str1());
    ASSERT(strcmp(*get_str3(), get_str1()) == 0);
    ASSERT(*get_int1ptr() == get_integer1());
    ASSERT(**get_int1ptr() == 0xfefea6);
    unsigned *(*someptr)() = get_integer1;
    ASSERT(*get_fnptr() == *(void **) &someptr);
    ASSERT(*get_null_ptr() == NULL);
    return EXIT_SUCCESS;
}
