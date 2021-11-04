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

unsigned int int_at(unsigned int x, unsigned int idx) {
    unsigned int array[x];
    if (((unsigned long) array) % _Alignof(int) != 0) {
        return 0;
    }

    for (unsigned int i = 0; i < x; i++) {
        array[i] = i + 1;
    }

    if (idx < x) {
        return array[idx];
    } else {
        return 0;
    }
}
