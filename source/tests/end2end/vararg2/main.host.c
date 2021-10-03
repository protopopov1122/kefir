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
    assert((int) sumall(0) == 0);
    assert((int) sumall(1, 10.0) == 10);
    assert((int) sumall(1, 5.0, 10.0) == 5);
    assert((int) sumall(1, 5.0f, 10.0) == 5);
    assert((int) sumall(2, 10.0, 20.0f) == 30);
    assert((int) sumall(3, 10.0, 20.0, 30.0) == 60);
    assert((int) sumall(4, 10.0f, 20.0, 30.0, 40.0f) == 100);
    assert((int) sumall(5, 10.0, 20.0f, 30.0, 40.0f, 50.0) == 150);
    assert((int) sumall(6, 10.0f, 20.0f, 30.0f, 40.0, 50.0f, 60.0) == 210);
    assert((int) sumall(7, 10.0, 20.0f, 30.0, 40.0f, 50.0, 60.0, 70.0f) == 280);
    assert((int) sumall(8, 10.0f, 20.0f, 30.0, 40.0, 50.0f, 60.0f, 70.0, 80.0f) == 360);
    assert((int) sumall(9, 10.0, 20.0, 30.0f, 40.0f, 50.0f, 60.0, 70.0, 80.0, 90.0f) == 450);
    assert((int) sumall(10, 10.0f, 20.0f, 30.0f, 40.0, 50.0, 60.0, 70.0f, 80.0, 90.0f, 100.0) == 550);
    return EXIT_SUCCESS;
}
