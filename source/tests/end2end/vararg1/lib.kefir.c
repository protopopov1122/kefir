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

#define INTERNAL
#include "./definitions.h"

int vsumall(int count, va_list vararg) {
    int sum = 0;
    while (count--) {
        int arg = va_arg(vararg, int);
        sum += arg;
    }
    return sum;
}

int sumall(int count, ...) {
    va_list vararg, vararg2;
    va_start(vararg, count);
    va_copy(vararg2, vararg);
    va_end(vararg);
    int sum = vsumall(count, vararg2);
    va_end(vararg2);
    return sum;
}
