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

#define DECLARE_CASE(case_name) \
    extern const struct kft_test_case case_name

#define DEFINE_CASE(case_name, case_description) \
    kefir_result_t run_test_##case_name(const struct kft_test_case *testCase, void *testContext); \
    const struct kft_test_case case_name = { .run = run_test_##case_name, .description = case_description }; \
    kefir_result_t run_test_##case_name(const struct kft_test_case *testCase, void *testContext) { \
        UNUSED(testCase); \
        UNUSED(testContext);

#define END_CASE \
        return KEFIR_OK; \
    }

#define TEST_SUITE(suite_name, ...) \
    const struct kft_test_case *suite_name[] = {__VA_ARGS__}; \
    const kefir_size_t suite_name##Length = sizeof(suite_name) / sizeof(suite_name[0])

#define ASSERT_(assertion, ret) \
    do { \
        if (!(assertion)) {\
            printf("%s:%d: Assertion `%s` failed\t", __FILE__, __LINE__, #assertion); \
            exit((ret)); \
        } \
    } while (0)

#define ASSERT(assertion) ASSERT_(assertion, KEFIR_UNKNOWN_ERROR)

#define ASSERT_OK(assertion) \
    do { \
        kefir_result_t _assert_ok_result = (assertion); \
        if (_assert_ok_result != KEFIR_OK) {\
            printf("%s:%d: `%s` did not return OK\t", __FILE__, __LINE__, #assertion); \
            exit((int) _assert_ok_result); \
        } \
    } while (0)

#define ASSERT_NOK(assertion) \
    do { \
        kefir_result_t _assert_ok_result = (assertion); \
        if (_assert_ok_result == KEFIR_OK) {\
            printf("%s:%d: `%s` returned OK\t", __FILE__, __LINE__, #assertion); \
            exit((int) _assert_ok_result); \
        } \
    } while (0)

#define FLOAT_EQUALS(x, y, epsilon) \
    (fabs((x) - (y)) <= (epsilon) || \
        (isinf((x)) && isinf((y)) && signbit((x)) == signbit((y))) || \
        (isnan((x)) && isnan((y))))
#define FLOAT_EPSILON 1e-5
#define DOUBLE_EQUALS(x, y, epsilon) \
    (fabs((x) - (y)) <= (epsilon) || \
        (isinf((x)) && isinf((y)) && signbit((x)) == signbit((y))) || \
        (isnan((x)) && isnan((y))))
#define DOUBLE_EPSILON 1e-10

#endif