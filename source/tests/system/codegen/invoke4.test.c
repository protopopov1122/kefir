/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

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

struct aggregate {
    int64_t array[4];
};

struct aggregate addstruct(int64_t x, struct aggregate a) {
    a.array[0] += x;
    a.array[1] += x;
    a.array[2] += x;
    a.array[3] += x;
    return a;
}

struct aggregate proxyadd(struct aggregate);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct aggregate agg = {.array = {i, i * 10, -i, i * (-10)}};
        struct aggregate res = proxyadd(agg);
        ASSERT(res.array[0] == i + 3);
        ASSERT(res.array[1] == i * 10 + 3);
        ASSERT(res.array[2] == -i + 3);
        ASSERT(res.array[3] == -i * 10 + 3);
    }
    return EXIT_SUCCESS;
}
