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

struct param {
    unsigned char a : 7;
    unsigned short b : 15;
    unsigned int c : 31;
    unsigned long d : 63;
    unsigned int e : 31;
    unsigned short f : 15;
    unsigned char g : 7;
    unsigned long h : 33;
    unsigned int i : 17;
    unsigned short j : 9;
};

void modify_and(struct param *, unsigned long);
void modify_or(struct param *, unsigned long);
void modify_xor(struct param *, unsigned long);
void modify_shl(struct param *, unsigned long);
void modify_shr(struct param *, unsigned long);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (unsigned int i = 0; i < 0xffe; i++) {
        struct param val = {
            .a = 100, .b = 1000, .c = 10000, .d = 100000, .e = 20000, .f = 2000, .g = 50, .h = 3000, .i = 300, .j = 30};

        unsigned long cnst = 0xcafebabeu ^ i;

        modify_and(&val, cnst);
        ASSERT(val.a == (100 & cnst));
        ASSERT(val.b == (1000 & cnst));
        ASSERT(val.c == (10000 & cnst));
        ASSERT(val.d == (100000 & cnst));
        ASSERT(val.e == (20000 & cnst));
        ASSERT(val.f == (2000 & cnst));
        ASSERT(val.g == (50 & cnst));
        ASSERT(val.h == (3000 & cnst));
        ASSERT(val.i == (300 & cnst));
        ASSERT(val.j == (30 & cnst));

        unsigned long cnst2 = cnst & 0x7f;
        modify_or(&val, cnst2);
        ASSERT(val.a == ((100 & cnst) | cnst2));
        ASSERT(val.b == ((1000 & cnst) | cnst2));
        ASSERT(val.c == ((10000 & cnst) | cnst2));
        ASSERT(val.d == ((100000 & cnst) | cnst2));
        ASSERT(val.e == ((20000 & cnst) | cnst2));
        ASSERT(val.f == ((2000 & cnst) | cnst2));
        ASSERT(val.g == ((50 & cnst) | cnst2));
        ASSERT(val.h == ((3000 & cnst) | cnst2));
        ASSERT(val.i == ((300 & cnst) | cnst2));
        ASSERT(val.j == ((30 & cnst) | cnst2));

        unsigned long cnst3 = (cnst2 ^ (~i)) & 0x7f;
        modify_xor(&val, cnst3);
        ASSERT(val.a == (((100 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.b == (((1000 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.c == (((10000 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.d == (((100000 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.e == (((20000 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.f == (((2000 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.g == (((50 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.h == (((3000 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.i == (((300 & cnst) | cnst2) ^ cnst3));
        ASSERT(val.j == (((30 & cnst) | cnst2) ^ cnst3));

        modify_shr(&val, 2);
        ASSERT(val.a == (((100 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.b == (((1000 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.c == (((10000 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.d == (((100000 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.e == (((20000 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.f == (((2000 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.g == (((50 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.h == (((3000 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.i == (((300 & cnst) | cnst2) ^ cnst3) >> 2);
        ASSERT(val.j == (((30 & cnst) | cnst2) ^ cnst3) >> 2);

        modify_shl(&val, 1);
        ASSERT(val.a == (((100 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.b == (((1000 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.c == (((10000 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.d == (((100000 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.e == (((20000 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.f == (((2000 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.g == (((50 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.h == (((3000 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.i == (((300 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
        ASSERT(val.j == (((30 & cnst) | cnst2) ^ cnst3) >> 2 << 1);
    }

    return EXIT_SUCCESS;
}
