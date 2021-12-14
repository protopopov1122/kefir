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

#define BASE 100

int ARRAY[10] = {
    (int) (&((int *) BASE)[0]), (int) (&((int *) BASE)[1]), (int) (&((int *) BASE)[2]), (int) (&((int *) BASE)[3]),
    (int) (&((int *) BASE)[4]), (int) (&((int *) BASE)[5]), (int) (&((int *) BASE)[6]), (int) (&((int *) BASE)[7]),
    (int) (&((int *) BASE)[8]), (int) (&((int *) BASE)[9]),
};
