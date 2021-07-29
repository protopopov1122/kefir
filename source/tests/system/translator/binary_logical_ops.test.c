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

typedef bool (*callback_t)(void *);

bool logical_and(callback_t, callback_t, void *);
bool logical_or(callback_t, callback_t, void *);

struct param {
    int fn_true;
    int fn_false;
};

bool fn_true(void *payload) {
    struct param *p = (struct param *) payload;
    p->fn_true++;
    return true;
}

bool fn_false(void *payload) {
    struct param *p = (struct param *) payload;
    p->fn_false++;
    return false;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

#define ASSERT_AND(_fn1, _fn2, _res, _true, _false)        \
    do {                                                   \
        struct param p = {0};                              \
        ASSERT(logical_and((_fn1), (_fn2), &p) == (_res)); \
        ASSERT(p.fn_true == (_true));                      \
        ASSERT(p.fn_false == (_false));                    \
    } while (0)

    ASSERT_AND(fn_false, fn_false, false, 0, 1);
    ASSERT_AND(fn_false, fn_true, false, 0, 1);
    ASSERT_AND(fn_true, fn_false, false, 1, 1);
    ASSERT_AND(fn_true, fn_true, true, 2, 0);

#define ASSERT_OR(_fn1, _fn2, _res, _true, _false)        \
    do {                                                  \
        struct param p = {0};                             \
        ASSERT(logical_or((_fn1), (_fn2), &p) == (_res)); \
        ASSERT(p.fn_true == (_true));                     \
        ASSERT(p.fn_false == (_false));                   \
    } while (0)

    ASSERT_OR(fn_false, fn_false, false, 0, 2);
    ASSERT_OR(fn_false, fn_true, true, 1, 1);
    ASSERT_OR(fn_true, fn_false, true, 1, 0);
    ASSERT_OR(fn_true, fn_true, true, 1, 0);
    return EXIT_SUCCESS;
}
