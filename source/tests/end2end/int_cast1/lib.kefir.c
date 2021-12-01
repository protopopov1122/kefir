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

long cast_uchar(long x) {
    return (unsigned char) x;
}

long cast_char(long x) {
    return (char) x;
}

long cast_ushort(long x) {
    return (unsigned short) x;
}

long cast_short(long x) {
    return (short) x;
}

long cast_uint(long x) {
    return (unsigned int) x;
}

long cast_int(long x) {
    return (int) x;
}
