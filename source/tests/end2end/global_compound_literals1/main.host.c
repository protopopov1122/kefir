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

int main() {
    assert(S1 != NULL);
    assert(S2 != NULL);
    assert(S3 != NULL);
    assert(S1 != S2);
    assert(S2 != (void *) S3);
    assert(S1 != (void *) S3);

    assert(S1->x == 100);
    assert(S1->y == -649);
    assert(fabs(S1->z - 3.14f) < 0.001f);

    assert(S2->x == -1);
    assert(S2->y == 0);
    assert(fabs(S2->z - 2.71f) < 0.001f);

    assert(fabs(S3->a - 1.2e12) < 0.001f);
    assert(S3->b == 'A');
    return EXIT_SUCCESS;
}
