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

struct sizes {
    int tboolean;
    int tchar;
    int tuchar;
    int tschar;
    int tushort;
    int tsshort;
    int tuint;
    int tsint;
    int tulong;
    int tslong;
    int tullong;
    int tsllong;
    int tfloat;
    int tdouble;
    int tpvoid;
    int tstruct;
    int tarray;
    int tstring;
};

struct sizes get_sizeof();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    struct sizes sz = get_sizeof();
    ASSERT(sz.tboolean == sizeof(bool));
    ASSERT(sz.tchar == sizeof(char));
    ASSERT(sz.tuchar == sizeof(unsigned char));
    ASSERT(sz.tschar == sizeof(signed char));
    ASSERT(sz.tushort == sizeof(unsigned short));
    ASSERT(sz.tsshort == sizeof(signed short));
    ASSERT(sz.tuint == sizeof(unsigned int));
    ASSERT(sz.tsint == sizeof(signed int));
    ASSERT(sz.tulong == sizeof(unsigned long));
    ASSERT(sz.tslong == sizeof(signed long));
    ASSERT(sz.tullong == sizeof(unsigned long long));
    ASSERT(sz.tsllong == sizeof(signed long long));
    ASSERT(sz.tfloat == sizeof(float));
    ASSERT(sz.tdouble == sizeof(double));
    ASSERT(sz.tpvoid == sizeof(void *));
    ASSERT(sz.tstruct == sizeof(struct sizes));
    ASSERT(sz.tarray == sizeof(int[128]));
    ASSERT(sz.tstring == sizeof("Hello, world!!!"));
    return EXIT_SUCCESS;
}
