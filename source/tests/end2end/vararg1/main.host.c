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

int main() {
    assert(sumall(0) == 0);
    assert(sumall(1, 10) == 10);
    assert(sumall(1, 5, 10) == 5);
    assert(sumall(2, 10, 20) == 30);
    assert(sumall(3, 10, 20, 30) == 60);
    assert(sumall(4, 10, 20, 30, 40) == 100);
    assert(sumall(5, 10, 20, 30, 40, 50) == 150);
    assert(sumall(6, 10, 20, 30, 40, 50, 60) == 210);
    assert(sumall(7, 10, 20, 30, 40, 50, 60, 70) == 280);
    assert(sumall(8, 10, 20, 30, 40, 50, 60, 70, 80) == 360);
    assert(sumall(9, 10, 20, 30, 40, 50, 60, 70, 80, 90) == 450);
    assert(sumall(10, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100) == 550);
    return EXIT_SUCCESS;
}
