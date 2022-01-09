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
#include "./definitions.h"

struct Test {
    long x;
    long y;
};

long sum(struct Test);

int main() {
    assert(INTEGER == 192739);
    assert(INTEGER2 == -282761);
    assert(sum((struct Test){INTEGER, INTEGER2}) == INTEGER + INTEGER2);
    assert(get_one() == 1);
    assert(get_two() == 2);
    return EXIT_SUCCESS;
}
