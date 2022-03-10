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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "./definitions.h"

STRUCT_S;

const S_t S_Instance = {0};

int main() {
    assert(AlignmentX[0] == _Alignof(S_t));
    assert(AlignmentX[1] == _Alignof(S_t));
    assert(SizeofX[0] == sizeof(S_t));
    assert(SizeofX[1] == sizeof(S_t));

    assert(AlignmentOfX(0) == _Alignof(S_t));
    assert(AlignmentOfX(1) == _Alignof(S_t));
    assert(SizeOfX(0) == sizeof(S_t));
    assert(SizeOfX(1) == sizeof(S_t));
    return EXIT_SUCCESS;
}
