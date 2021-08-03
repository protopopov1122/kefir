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
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

void copybool(const bool *, bool *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    bool b1 = false, b2 = true;

    REQUIRE(b1 == false, KEFIR_INTERNAL_ERROR);
    copybool(&(bool){true}, &b1);
    REQUIRE(b1 == true, KEFIR_INTERNAL_ERROR);

    REQUIRE(b2 == true, KEFIR_INTERNAL_ERROR);
    copybool(&(bool){false}, &b2);
    REQUIRE(b2 == false, KEFIR_INTERNAL_ERROR);
    return EXIT_SUCCESS;
}
