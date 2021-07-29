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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    int f1;
    struct {
        float f2;
        union {
            char c[8];
            long i64;
        } f3[10];
        int64_t f4;
    };
};

long *arrptr(struct type1 *, size_t);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (kefir_int64_t i = -1000; i < 1000; i++) {
        struct type1 val;
        for (kefir_size_t j = 0; j < 10; j++) {
            *arrptr(&val, j) = i + j;
        }
        for (kefir_size_t j = 0; j < 10; j++) {
            ASSERT(val.f3[j].i64 == (long) (i + j));
        }
    }
    return EXIT_SUCCESS;
}
