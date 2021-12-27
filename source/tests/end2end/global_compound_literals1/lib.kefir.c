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

struct Struct1 *S1 = &(struct Struct1){.x = 100, .y = -649, .z = 3.14f};

struct Struct1 *S2 = &(struct Struct1){.x = -1, .y = 0, .z = 2.71f};

struct Struct2 *S3 = &(struct Struct2){.a = 1.2e12, .b = 'A'};
