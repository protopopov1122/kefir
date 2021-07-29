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

long value_to_add = 0;

void set_value(long);
long get_value();
void add_value();
long invoke_callback();

static long offset = 0;
long value_callback(long value) {
    return value + offset;
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    for (long l = -100; l < 100; l++) {
        set_value(l);
        ASSERT(get_value() == l);
        for (long k = 100; k > -100; k--) {
            set_value(l ^ k);
            ASSERT(get_value() == (l ^ k));
        }
    }

    set_value(0);
    long sum = 0;
    for (long l = -100; l < 100; l++) {
        sum += l;
        value_to_add = l;
        add_value();
        ASSERT(get_value() == sum);
    }

    for (long l = -100; l < 100; l++) {
        set_value(l);
        ASSERT(get_value() == l);
        for (long k = 100; k > -100; k--) {
            offset = k;
            ASSERT(invoke_callback() == l + k);
        }
    }
    return EXIT_SUCCESS;
}
