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

typedef struct S S_t;
#define STRUCT_S    \
    struct S {      \
        char a[24]; \
        long b;     \
        int c;      \
        double d;   \
        char e;     \
        float f;    \
    }

extern const S_t S_Instance;
extern const unsigned int AlignmentX[2];
extern const unsigned int SizeofX[2];

extern unsigned int AlignmentOfX(int);
extern unsigned int SizeOfX(int);

#endif
