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

void fillzeros(long *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    for (kefir_size_t i = 1; i < 1000; i++) {
        long l = i;
        fillzeros(&l);
        REQUIRE(l == 0, KEFIR_INTERNAL_ERROR);
    }
    return EXIT_SUCCESS;
}
