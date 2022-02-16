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

#ifndef KEFIR_TEST_UNIT_TEST_H_
#define KEFIR_TEST_UNIT_TEST_H_

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/core/mem.h"

extern struct kefir_mem kft_mem;

typedef struct kft_test_case {
    kefir_result_t (*run)(const struct kft_test_case *, void *);
    const char *description;
} kft_test_case_t;

kefir_result_t kft_run_test_case(const struct kft_test_case *, void *);
kefir_size_t kft_run_test_suite(const struct kft_test_case **, kefir_size_t, void *);

#define DECLARE_CASE(case_name) extern const struct kft_test_case case_name

#define DEFINE_CASE(case_name, case_description)                                                           \
    kefir_result_t run_test_##case_name(const struct kft_test_case *testCase, void *testContext);          \
    const struct kft_test_case case_name = {.run = run_test_##case_name, .description = case_description}; \
    kefir_result_t run_test_##case_name(const struct kft_test_case *testCase, void *testContext) {         \
        UNUSED(testCase);                                                                                  \
        UNUSED(testContext);                                                                               \
        do

#define END_CASE     \
    while (0)        \
        ;            \
    return KEFIR_OK; \
    }

#define TEST_SUITE(suite_name, ...)                           \
    const struct kft_test_case *suite_name[] = {__VA_ARGS__}; \
    const kefir_size_t suite_name##Length = sizeof(suite_name) / sizeof(suite_name[0])

#define ASSERT_(assertion, ret)                                                       \
    do {                                                                              \
        if (!(assertion)) {                                                           \
            printf("%s:%d: Assertion `%s` failed\t", __FILE__, __LINE__, #assertion); \
            exit((ret));                                                              \
        }                                                                             \
    } while (0)

#define ASSERT(assertion) ASSERT_(assertion, KEFIR_UNKNOWN_ERROR)

#define ASSERT_OK(assertion)                                                           \
    do {                                                                               \
        kefir_result_t _assert_ok_result = (assertion);                                \
        if (_assert_ok_result != KEFIR_OK) {                                           \
            printf("%s:%d: `%s` did not return OK\t", __FILE__, __LINE__, #assertion); \
            exit((int) _assert_ok_result);                                             \
        }                                                                              \
    } while (0)

#define ASSERT_NOK(assertion)                                                    \
    do {                                                                         \
        kefir_result_t _assert_ok_result = (assertion);                          \
        if (_assert_ok_result == KEFIR_OK) {                                     \
            printf("%s:%d: `%s` returned OK\t", __FILE__, __LINE__, #assertion); \
            exit(-1);                                                            \
        }                                                                        \
    } while (0)

#define FLOAT_EQUALS(x, y, epsilon)                                                                \
    (fabs((x) - (y)) <= (epsilon) || (isinf((x)) && isinf((y)) && signbit((x)) == signbit((y))) || \
     (isnan((x)) && isnan((y))))
#define FLOAT_EPSILON 1e-5
#define DOUBLE_EQUALS(x, y, epsilon)                                                               \
    (fabs((x) - (y)) <= (epsilon) || (isinf((x)) && isinf((y)) && signbit((x)) == signbit((y))) || \
     (isnan((x)) && isnan((y))))
#define DOUBLE_EPSILON 1e-10
#define LONG_DOUBLE_EQUALS(x, y, epsilon)                                                           \
    (fabsl((x) - (y)) <= (epsilon) || (isinf((x)) && isinf((y)) && signbit((x)) == signbit((y))) || \
     (isnan((x)) && isnan((y))))
#define LONG_DOUBLE_EPSILON 1e-10

#endif
