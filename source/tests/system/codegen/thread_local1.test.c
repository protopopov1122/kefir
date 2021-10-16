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
#include <threads.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"

struct type1 {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
};

static const char ZEROS[sizeof(struct type1)] = {0};
_Thread_local struct type1 thrlocal1 = {0};
void fill(uint64_t);

static void test_struct(struct type1 *value) {
    for (kefir_int64_t i = -100; i < 100; i++) {
        fill(0xf12345678ul + i);
        ASSERT(value->u8 == ((uint8_t) 0x78) + i);
        ASSERT(value->u16 == ((uint16_t) 0x5678) + i);
        ASSERT(value->u32 == ((uint32_t) 0x12345678) + i);
        ASSERT(value->u64 == 0xf12345678ul + i);
    }
}

static int thread2(void *payload) {
    struct type1 *Original = payload;
    ASSERT(&thrlocal1 != Original);
    ASSERT(memcmp(&thrlocal1, ZEROS, sizeof(struct type1)) == 0);
    test_struct(&thrlocal1);
    ASSERT(memcmp(&thrlocal1, ZEROS, sizeof(struct type1)) != 0);
    return sizeof(struct type1);
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(memcmp(&thrlocal1, ZEROS, sizeof(struct type1)) == 0);
    test_struct(&thrlocal1);
    ASSERT(memcmp(&thrlocal1, ZEROS, sizeof(struct type1)) != 0);

    thrd_t thread;
    ASSERT(thrd_create(&thread, thread2, &thrlocal1) == thrd_success);
    int res = 0;
    ASSERT(thrd_join(thread, &res) == thrd_success);
    ASSERT(res == sizeof(struct type1));
    return EXIT_SUCCESS;
}
