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

static unsigned call1 = 0;
static unsigned call2 = 0;

int callback1() {
    call1++;
    return 1;
}

long callback2() {
    call2++;
    return 2;
}

int main() {
    run_callbacks(NULL, NULL, NULL);
    assert(call1 == 1);
    assert(call2 == 1);
    run_callbacks(NULL, NULL, NULL);
    run_callbacks(NULL, NULL, NULL);
    run_callbacks(NULL, NULL, NULL);
    assert(call1 == 4);
    assert(call2 == 4);
    run_callbacks(NULL, NULL, NULL);
    assert(call1 == 5);
    assert(call2 == 5);
    call1 = 0;
    call2 = 0;
    run_callbacks(NULL, NULL, NULL);
    assert(call1 == 1);
    assert(call2 == 1);

    run_callbacks2();
    assert(call1 == 1);
    assert(call2 == 3);
    return EXIT_SUCCESS;
}
