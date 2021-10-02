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

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#ifdef INTERNAL
typedef unsigned short char16_t;
typedef unsigned int char32_t;
typedef int wchar_t;
#endif

extern const char Literal1[];
extern const char16_t Literal2[];
extern const char32_t Literal3[];
extern const wchar_t Literal4[];

struct u8string {
    char content[24];
};

void init_u8str(struct u8string *);

struct u16string {
    char16_t content[24];
};

void init_u16str(struct u16string *);

struct u32string {
    char32_t content[24];
};

void init_u32str(struct u32string *);

struct lstring {
    wchar_t content[24];
};

void init_lstr(struct lstring *);

#endif
