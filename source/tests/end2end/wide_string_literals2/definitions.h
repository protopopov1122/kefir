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

struct content {
    const char16_t *literal1;
    char16_t literal2[32];
    const char32_t *literal3;
    char32_t literal4[32];
    const wchar_t *literal5;
    wchar_t literal6[32];
};

struct content init_content();

#endif
