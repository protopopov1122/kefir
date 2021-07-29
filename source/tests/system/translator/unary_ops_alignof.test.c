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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct aligns {
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
};

struct aligns get_alignof();

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    struct aligns sz = get_alignof();
    ASSERT(sz.tboolean == _Alignof(bool));
    ASSERT(sz.tchar == _Alignof(char));
    ASSERT(sz.tuchar == _Alignof(unsigned char));
    ASSERT(sz.tschar == _Alignof(signed char));
    ASSERT(sz.tushort == _Alignof(unsigned short));
    ASSERT(sz.tsshort == _Alignof(signed short));
    ASSERT(sz.tuint == _Alignof(unsigned int));
    ASSERT(sz.tsint == _Alignof(signed int));
    ASSERT(sz.tulong == _Alignof(unsigned long));
    ASSERT(sz.tslong == _Alignof(signed long));
    ASSERT(sz.tullong == _Alignof(unsigned long long));
    ASSERT(sz.tsllong == _Alignof(signed long long));
    ASSERT(sz.tfloat == _Alignof(float));
    ASSERT(sz.tdouble == _Alignof(double));
    ASSERT(sz.tpvoid == _Alignof(void *));
    ASSERT(sz.tstruct == _Alignof(struct aligns));
    ASSERT(sz.tarray == _Alignof(int[128]));
    return EXIT_SUCCESS;
}
