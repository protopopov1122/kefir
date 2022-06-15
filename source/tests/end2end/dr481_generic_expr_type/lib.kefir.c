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

extern const int Values[] = {
    _Generic((const int){0}, int : 1, const int : 2, const volatile int : 3, default : 4),
    _Generic((const volatile int){10}, int : 1, const int : 2, default : 3),
    _Generic((const volatile double){1000}, int : 1, const int : 2, default : 3),
    _Generic((const volatile double){10000}, int : 1, const int : 2, double : 101, default : 3),
    _Generic((const long *volatile){0}, long : 1, const long : 2, long * : 3, const long * : 4,
             const long *volatile : 5, default : 6)};
