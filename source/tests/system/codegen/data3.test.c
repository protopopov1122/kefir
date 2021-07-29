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
    uint8_t u8;
    int8_t i8;
    uint16_t u16;
    int16_t i16;
    uint32_t u32;
    int32_t i32;
    int64_t i64;

    int64_t result;
};

const int64_t *sum(struct type1 *);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (int64_t i = -100; i < 100; i++) {
        struct type1 val = {.u8 = (uint8_t) (i + 100),
                            .i8 = i,
                            .u16 = (uint16_t) (i + 1000),
                            .i16 = (int16_t) (-i * 10),
                            .u32 = (uint32_t) (i + 0xffff),
                            .i32 = i * 1000,
                            .i64 = i + 0xffffffffl};
        const int64_t *res = sum(&val);
        ASSERT(*res == i + 100 + i + i + 1000 + (-i * 10) + i + 0xffff + i * 1000 + i + 0xffffffffl);
        ASSERT(*res == val.result);
        ASSERT(res == &val.result);
    }
    return EXIT_SUCCESS;
}
