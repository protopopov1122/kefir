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

#include "./definitions.h"

unsigned int func1(unsigned int idx) {
    void *array[] = {&&A, &&B, &&C, &&D, &&E, &&F, &&G, &&H, &&I, &&J};
    goto *array[idx - 1];

A:
    return 10;
B:
    return 20;
C:
    return 30;
D:
    return 40;
E:
    return 50;
F:
    return 60;
G:
    return 70;
H:
    return 80;
I:
    return 90;
J:
    return 100;
}
