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

union Union1 {
    int f1;
    long f2 : 33;
    unsigned short f3 : 12;
    struct {
        unsigned int f4;
        unsigned long f5 : 30;
        unsigned long f6 : 15;
    };
    char f7 : 7;
    long f8 : 20;
};

extern unsigned int Uniont1Size;
extern unsigned int Uniont1Alignment;

union Union1 fillu(int);

#endif
