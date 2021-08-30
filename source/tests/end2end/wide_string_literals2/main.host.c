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

struct content {
    const char16_t *literal1;
    char16_t literal2[32];
    const char32_t *literal3;
    char32_t literal4[32];
    const wchar_t *literal5;
    wchar_t literal6[32];
};

struct content init_content();

int main() {
    const char16_t Expected1[] = u"String literal#1.";
    const char16_t Expected2[] = u"String literal#2.";
    const char32_t Expected3[] = U"String literal#3.";
    const char32_t Expected4[] = U"String literal#4.";
    const wchar_t Expected5[] = L"String literal#5.";
    const wchar_t Expected6[] = L"String literal#6.";

    struct content c = init_content();
    assert(memcmp(c.literal1, Expected1, sizeof(Expected1)) == 0);
    assert(memcmp(c.literal2, Expected2, sizeof(Expected2)) == 0);
    assert(memcmp(c.literal3, Expected3, sizeof(Expected3)) == 0);
    assert(memcmp(c.literal4, Expected4, sizeof(Expected4)) == 0);
    assert(memcmp(c.literal5, Expected5, sizeof(Expected5)) == 0);
    assert(memcmp(c.literal6, Expected6, sizeof(Expected6)) == 0);
    return EXIT_SUCCESS;
}