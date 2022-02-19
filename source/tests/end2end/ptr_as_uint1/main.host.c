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
#include <assert.h>
#include <math.h>
#include <string.h>
#include "./definitions.h"

int Value = 0;

int main() {
    assert(Ptr1 == ((long long) &Value) - 1000);
    assert(Ptr2 == (unsigned long) (&Value + 245));
    assert(Ptr3 == ((long) &Value) + 2000 - 1);
    assert(strcmp((const char *) (Str1 + 0x200000ull), "Hello, cruel world!") == 0);
    return EXIT_SUCCESS;
}
