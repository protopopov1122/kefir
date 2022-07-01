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
#include <assert.h>
#include "kefir/util/uchar.h"
#include "./definitions.h"

int main() {
    const wchar_t wexpected[] = L"Hello, world!";
    const char16_t c16expected[] = u"Hello, world!";
    const char32_t c32expected[] = U"Hello, world!";
    struct wide_string wstr;
    struct char16_string c16str;
    struct char32_string c32str;
    wide_greet(&wstr);
    char16_greet(&c16str);
    char32_greet(&c32str);
    for (size_t i = 0; i < sizeof(wexpected) / sizeof(wexpected[0]); i++) {
        assert(wstr.content[i] == wexpected[i]);
    }
    for (size_t i = 0; i < sizeof(c16expected) / sizeof(c16expected[0]); i++) {
        assert(c16str.content[i] == c16expected[i]);
    }
    for (size_t i = 0; i < sizeof(c32expected) / sizeof(c32expected[0]); i++) {
        assert(c32str.content[i] == c32expected[i]);
    }
    return EXIT_SUCCESS;
}
