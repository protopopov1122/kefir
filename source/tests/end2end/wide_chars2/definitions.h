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

struct wide_string {
    wchar_t content[32];
};

struct char16_string {
    char16_t content[32];
};

struct char32_string {
    char32_t content[32];
};

void wide_greet(struct wide_string *);
void char16_greet(struct char16_string *);
void char32_greet(struct char32_string *);

#endif
