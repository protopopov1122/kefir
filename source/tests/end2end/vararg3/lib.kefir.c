/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

int vzeros(va_list vararg) {
    int count = 0;
    for (int i = va_arg(vararg, int); i == 0; ++count, i = va_arg(vararg, int))
        ;
    return count;
}

int zeros(int arg, ...) {
    (void) arg;

    va_list vararg;
    va_start(vararg, arg);
    int count = vzeros(vararg);
    int next = va_arg(vararg, int);
    va_end(vararg);
    return count + next;
}
