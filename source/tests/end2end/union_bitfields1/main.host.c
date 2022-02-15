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
#include <string.h>
#include "./definitions.h"

int main() {
    assert(Uniont1Size == sizeof(union Union1));
    assert(Uniont1Alignment == _Alignof(union Union1));
    for (int i = -100; i < 100; i++) {
        union Union1 u1;
        memset(&u1, 0, sizeof(u1));
        u1.f1 = i;

        union Union1 u2 = fillu(i);
        assert(memcmp(&u1, &u2, sizeof(u1)) == 0);
    }
    return EXIT_SUCCESS;
}
