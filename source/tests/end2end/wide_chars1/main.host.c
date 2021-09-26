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
#include "./definitions.h"

int main() {
    wchar_t wstr[] = L"abcdefghijklmnopqrstuvwxyz"
                     L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wchar_t wexpected[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    _Static_assert(sizeof(wstr) == sizeof(wexpected), "Char array length mismatch");

    char16_t u16str[] = u"abcdefghijklmnopqrstuvwxyz"
                        u"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char16_t u16expected[] = u"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   u"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    _Static_assert(sizeof(u16str) == sizeof(u16expected), "Char array length mismatch");

    char32_t u32str[] = U"abcdefghijklmnopqrstuvwxyz"
                        U"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char32_t u32expected[] = U"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   U"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    _Static_assert(sizeof(u32str) == sizeof(u32expected), "Char array length mismatch");

    wstring_uppercase(wstr);
    for (size_t i = 0; i < sizeof(wstr) / sizeof(wchar_t); i++) {
        assert(wstr[i] == wexpected[i]);
    }

    u16string_uppercase(u16str);
    for (size_t i = 0; i < sizeof(u16str) / sizeof(char16_t); i++) {
        assert(u16str[i] == u16expected[i]);
    }

    u32string_uppercase(u32str);
    for (size_t i = 0; i < sizeof(u32str) / sizeof(char32_t); i++) {
        assert(u32str[i] == u32expected[i]);
    }
    return EXIT_SUCCESS;
}
