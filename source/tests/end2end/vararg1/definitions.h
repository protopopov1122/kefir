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
#define va_list __builtin_va_list
#define va_start(vararg, paramN) __builtin_va_start((vararg), paramN)
#define va_arg(vararg, type) __builtin_va_arg((vararg), type)
#define va_copy(dest, src) __builtin_va_copy((dest), (src))
#define va_end(vararg) __builtin_va_end((vararg))
#else
#include <stdarg.h>
#endif

int sumall(int, ...);
int vsumall(int, va_list);

#endif
