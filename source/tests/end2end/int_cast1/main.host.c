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
#include "./definitions.h"

int main() {
    const long Offset = -0x1ffffffffll;
    for (long i = 0; i < 0xffff; i++) {
        long x = i + Offset;
        assert(cast_uchar(x) == (unsigned char) x);
        assert(cast_char(x) == (char) x);
        assert(cast_ushort(x) == (unsigned short) x);
        assert(cast_short(x) == (short) x);
        assert(cast_uint(x) == (unsigned int) x);
        assert(cast_int(x) == (int) x);
    }
    return EXIT_SUCCESS;
}
