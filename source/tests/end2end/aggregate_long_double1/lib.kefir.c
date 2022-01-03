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

#define va_list __builtin_va_list
#define va_start(vararg, paramN) __builtin_va_start((vararg), paramN)
#define va_arg(vararg, type) __builtin_va_arg((vararg), type)
#define va_copy(dest, src) __builtin_va_copy((dest), (src))
#define va_end(vararg) __builtin_va_end((vararg))

struct Param1 ldneg(struct Param1 p) {
    return (struct Param1){-p.value};
}

struct Param1 ldsum(struct Param2 p) {
    return (struct Param1){p.x + p.y};
}

struct Param1 ldvsum(int num, ...) {
    long double x = 0.0;
    va_list list;
    va_start(list, num);
    while (num--) {
        x += va_arg(list, long double);
    }
    va_end(list);
    return (struct Param1){x};
}

struct Param1 ldvsum2(int num, ...) {
    long double x = 0.0;
    va_list list;
    va_start(list, num);
    while (num--) {
        struct Param1 p = va_arg(list, struct Param1);
        x += p.value;
    }
    va_end(list);
    return (struct Param1){x};
}

long double ldunwrap(struct Param1 (*fn)()) {
    return fn().value;
}
