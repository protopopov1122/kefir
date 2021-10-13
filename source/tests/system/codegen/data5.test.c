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

extern _Thread_local int8_t int8_1, int8_2;
extern _Thread_local int16_t int16_1, int16_2;
extern _Thread_local int32_t int32_1, int32_2;
extern _Thread_local int64_t int64_1, int64_2;
extern _Thread_local float float32_1, float32_2;
extern _Thread_local double float64_1, float64_2;
extern _Thread_local struct {
    int8_t f1;
    int64_t f2;
    int16_t f3;
    float f4;
} struct1_1;
extern _Thread_local struct {
    char f1[2];
    float f2[2];
} array1_1[3];

union union1 {
    struct {
        float f2;
        int f3;
    };
    char f1[8];
};
extern _Thread_local union union1 union1_1;

extern _Thread_local const char memory1_1[];
extern _Thread_local const char pad1_1[];
extern _Thread_local const char *pointer1_1;
extern _Thread_local const char *strpointer1_1;

#define PTRLIST                                                                                                        \
    {                                                                                                                  \
        &int8_1, &int8_2, &int16_1, &int16_2, &int32_1, &int32_2, &int64_1, &int64_2, &float32_1, &float32_2,          \
            &float64_1, &float64_2, &struct1_1, &array1_1, &union1_1, &memory1_1, &pad1_1, &pointer1_1, &strpointer1_1 \
    }

static int cmp_pointers(void *payload) {
    const void **Original = payload;
    const void *New[] = PTRLIST;
    kefir_size_t length = sizeof(New) / sizeof(New[0]);
    for (kefir_size_t i = 0; i < length; i++) {
        ASSERT(Original[i] != New[i]);
    }
    return length;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    ASSERT(int8_1 == 104);
    ASSERT(int8_2 == 0);
    ASSERT(int16_1 == 2589);
    ASSERT(int16_2 == 0);
    ASSERT(int32_1 == 0x1ffff);
    ASSERT(int32_2 == 0);
    ASSERT(int64_1 == 0x1ffffffff);
    ASSERT(int64_2 == 0);
    ASSERT(FLOAT_EQUALS(float32_1, 3.14159f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(float32_2, 0.0f, FLOAT_EPSILON));
    ASSERT(DOUBLE_EQUALS(float64_1, 2.718281828, DOUBLE_EPSILON));
    ASSERT(DOUBLE_EQUALS(float64_2, 0.0, DOUBLE_EPSILON));
    ASSERT(struct1_1.f1 == 127);
    ASSERT(struct1_1.f2 == 0x2ffffffff);
    ASSERT(struct1_1.f3 == 4096);
    ASSERT(FLOAT_EQUALS(struct1_1.f4, 106.9994, FLOAT_EPSILON));
    ASSERT(array1_1[0].f1[0] == 10);
    ASSERT(array1_1[0].f1[1] == 20);
    ASSERT(FLOAT_EQUALS(array1_1[0].f2[0], 3.4f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array1_1[0].f2[1], 4.5f, FLOAT_EPSILON));
    ASSERT(array1_1[1].f1[0] == 0);
    ASSERT(array1_1[1].f1[1] == 0);
    ASSERT(FLOAT_EQUALS(array1_1[1].f2[0], 0.0f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array1_1[1].f2[1], 0.0f, FLOAT_EPSILON));
    ASSERT(array1_1[2].f1[0] == 110);
    ASSERT(array1_1[2].f1[1] == 120);
    ASSERT(FLOAT_EQUALS(array1_1[2].f2[0], 40.56f, FLOAT_EPSILON));
    ASSERT(FLOAT_EQUALS(array1_1[2].f2[1], 56.74f, FLOAT_EPSILON));

    union union1 union1_1_copy;
    memcpy(union1_1_copy.f1, union1_1.f1, 8);
    ASSERT(FLOAT_EQUALS(union1_1_copy.f2, 3.14, FLOAT_EPSILON));
    ASSERT(union1_1_copy.f3 == 100500);

    for (int i = 0; i < 10; i++) {
        ASSERT(pad1_1[i] == 0);
    }
    ASSERT(strcmp(pointer1_1, "llo, cruel world!") == 0);
    ASSERT(strcmp(strpointer1_1, ", cruel world!") == 0);

    const void *Pointers[] = PTRLIST;
    thrd_t thread;
    ASSERT(thrd_create(&thread, cmp_pointers, Pointers) == thrd_success);
    int res = 0;
    ASSERT(thrd_join(thread, &res) == thrd_success);
    ASSERT(res == sizeof(Pointers) / sizeof(Pointers[0]));
    return EXIT_SUCCESS;
}
