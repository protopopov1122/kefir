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

#include "./definitions.h"

long long Ptr1 = ((long long) &Value) - 1000;
unsigned long Ptr2 = (unsigned long) (&Value + 245);
long Ptr3 = ((long) &Value) + 2000 - 1;

unsigned long long Str1 = ((unsigned long long) "Hello, cruel world!") - 0x200000ull;