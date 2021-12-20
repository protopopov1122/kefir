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

struct Struct1 {
    char c1;
    char c2;
    short s1;
    int i1;
    int i2;
    int i3;
    union {
        int i4;
        int i5;
        void *w1;
    };
};

#define STRUCT1_CALCULATE(s) ((long) ((s)->c1 + (s)->c2 + (s)->s1 + (s)->i1 + (s)->i2 + (s)->i3 + (s)->i4))
long struct1_calculate(struct Struct1);

#endif
