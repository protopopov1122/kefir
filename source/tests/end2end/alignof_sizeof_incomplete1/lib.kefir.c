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

STRUCT_S;

const unsigned int AlignmentX[2] = {_Alignof(S_t), _Alignof(S_Instance)};
const unsigned int SizeofX[2] = {sizeof(S_t), sizeof S_Instance};

unsigned int AlignmentOfX(int idx) {
    if (idx) {
        return _Alignof(S_t);
    } else {
        return _Alignof(S_Instance);
    }
}

unsigned int SizeOfX(int idx) {
    if (idx) {
        return sizeof(S_t);
    } else {
        return sizeof S_Instance;
    }
}
