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

int64_t sumstruct(int64_t x, struct aggregate a) {
    return x * (a.array[0] + a.array[1] + a.array[2] + a.array[3]);
}

int64_t proxysum(struct aggregate);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    struct aggregate agg;
    for (int64_t i = -100; i < 100; i++) {
        for (int64_t j = -100; j < 100; j++) {
            agg.array[0] = i;
            agg.array[1] = j;
            agg.array[2] = i * j;
            agg.array[3] = i ^ j;
            ASSERT(proxysum(agg) == 2 * (i + j + i * j + (i ^ j)));
        }
    }
    return EXIT_SUCCESS;
}
