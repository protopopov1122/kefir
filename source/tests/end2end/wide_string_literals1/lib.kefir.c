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

#define INTERNAL
#include "./definitions.h"

const char Literal1[] = u8"Test...test..test...\0u8-prefixed string\r\n\0?";
const char16_t Literal2[] = u"x\\\"16-bit\0string\0literal";
const char32_t Literal3[] = U"32-bit string    \t\t\v literal is here\0\0\0";
const wchar_t Literal4[] = L"Wide character\n\r\nis defined as 32-bit char\0\00\000\x0...";

void init_u8str(struct u8string *str) {
    *str = (struct u8string){u8"Hello, world!"};
}

void init_u16str(struct u16string *str) {
    *str = (struct u16string){u"Hello, cruel world!"};
}

void init_u32str(struct u32string *str) {
    *str = (struct u32string){U"Goodbye, cruel world!"};
}

void init_lstr(struct lstring *str) {
    *str = (struct lstring){L"It\'s me, world!"};
}