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

#define EXPR1 ((~(unsigned int) 0) / 17)
#define EXPR2 (((long long) ~5ull) >> 7)
#define EXPR3 ((((unsigned short) -100) ^ 0x8162) >> (-3353 & 2))
#define EXPR4 ((long) ~((((unsigned int) 172.28e1f) / 3uLL) << 4))
#define EXPR5 ((float) (~((unsigned long) -1826L) << 3))
#define EXPR6 ((double) ((((~0uLL) > 0) << 3) | (-2 < ~0LL)))
#define EXPR7 (-((~(((unsigned long) 1827.387e-1L) << 1) | (0u == (-1 + 1uLL))) << 1))
#define EXPR8 ((unsigned long) (((char *) 100) - ((char *) 0)))
#define EXPR9 (MEMBERSZ(struct S1, c))

#define MEMBERSZ(type, member) ((unsigned long) ((char *) &(((type *) 0)->member) - (char *) 0))

struct S1 {
    void *a;
    char b;
    long c;
};

extern unsigned short X1;
extern long long X2;
extern unsigned char X3;
extern long X4;
extern float X5;
extern double X6;
extern int X7;
extern unsigned long X8;
extern unsigned long X9;

#endif
