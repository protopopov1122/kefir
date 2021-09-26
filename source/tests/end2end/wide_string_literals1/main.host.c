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
#include <wchar.h>
#include <uchar.h>
#include <assert.h>
#include <string.h>
#include "./definitions.h"

int main() {
    const char Expected1[] = u8"Test...test..test...\0u8-prefixed string\r\n\0?";
    const char16_t Expected2[] = u"x\\\"16-bit\0string\0literal";
    const char32_t Expected3[] = U"32-bit string    \t\t\v literal is here\0\0\0";
    const wchar_t Expected4[] = L"Wide character\n\r\nis defined as 32-bit char\0\00\000\x0...";

    assert(memcmp(Literal1, Expected1, sizeof(Expected1)) == 0);
    assert(memcmp(Literal2, Expected2, sizeof(Expected2)) == 0);
    assert(memcmp(Literal3, Expected3, sizeof(Expected3)) == 0);
    assert(memcmp(Literal4, Expected4, sizeof(Expected4)) == 0);

    const char Expected5[] = u8"Hello, world!";
    struct u8string u8str;
    init_u8str(&u8str);
    assert(memcmp(u8str.content, Expected5, sizeof(Expected5)) == 0);

    const char16_t Expected6[] = u"Hello, cruel world!";
    struct u16string u16str;
    init_u16str(&u16str);
    assert(memcmp(u16str.content, Expected6, sizeof(Expected6)) == 0);

    const char32_t Expected7[] = U"Goodbye, cruel world!";
    struct u32string u32str;
    init_u32str(&u32str);
    assert(memcmp(u32str.content, Expected7, sizeof(Expected7)) == 0);

    const wchar_t Expected8[] = L"It\'s me, world!";
    struct lstring lstr;
    init_lstr(&lstr);
    assert(memcmp(lstr.content, Expected8, sizeof(Expected8)) == 0);
    return EXIT_SUCCESS;
}
