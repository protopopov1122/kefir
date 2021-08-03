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
#include <string.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
};

void truncate(struct type1 *, uint64_t);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    struct type1 val;
    truncate(&val, 0xf12345678ul);
    ASSERT(val.u8 == 0x78);
    ASSERT(val.u16 == 0x5678);
    ASSERT(val.u32 == 0x12345678);
    ASSERT(val.u64 == 0xf12345678);
    return EXIT_SUCCESS;
}
