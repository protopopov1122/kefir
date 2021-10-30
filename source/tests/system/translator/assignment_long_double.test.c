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

long double assign(long double *, long double);
long double multiply_assign(long double *, long double);
long double divide_assign(long double *, long double);
long double add_assign(long double *, long double);
long double sub_assign(long double *, long double);

static void simple_assign() {
    long double ld;
    for (long double x = -100.0l; x < 100.0l; x += 0.01l) {
        long double res = assign(&ld, x);
        ASSERT(LONG_DOUBLE_EQUALS(res, x, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(ld, x, LONG_DOUBLE_EPSILON));
    }
}

static void mul_assign() {
    long double mul = 1.0l;
    long double ld = 1.0l;
    for (long double x = 1.0l; x < 3.0l; x += 0.01l) {
        mul *= x;
        long double res = multiply_assign(&ld, x);
        ASSERT(LONG_DOUBLE_EQUALS(res, mul, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(ld, mul, LONG_DOUBLE_EPSILON));
    }
}

static void div_assign() {
    long double mul = 1000.0l;
    long double ld = 1000.0l;
    for (long double x = 1.0l; x < 3.0l; x += 0.01l) {
        mul /= x;
        long double res = divide_assign(&ld, x);
        ASSERT(LONG_DOUBLE_EQUALS(res, mul, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(ld, mul, LONG_DOUBLE_EPSILON));
    }
}

static void sum_assign() {
    long double sum = 0.0l;
    long double ld = 0.0l;
    for (long double x = -10.0l; x < 100.0l; x += 0.01l) {
        sum += x;
        long double res = add_assign(&ld, x);
        ASSERT(LONG_DOUBLE_EQUALS(res, sum, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(ld, sum, LONG_DOUBLE_EPSILON));
    }
}

static void subtract_assign() {
    long double sum = 1000.0l;
    long double ld = 1000.0l;
    for (long double x = -10.0l; x < 100.0l; x += 0.01l) {
        sum -= x;
        long double res = sub_assign(&ld, x);
        ASSERT(LONG_DOUBLE_EQUALS(res, sum, LONG_DOUBLE_EPSILON));
        ASSERT(LONG_DOUBLE_EQUALS(ld, sum, LONG_DOUBLE_EPSILON));
    }
}

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    simple_assign();
    mul_assign();
    div_assign();
    sum_assign();
    subtract_assign();
    return EXIT_SUCCESS;
}
