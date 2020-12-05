#ifndef KEFIR_CORE_UTIL_H_
#define KEFIR_CORE_UTIL_H_

#include <limits.h>
#include "kefir/core/basic-types.h"

#define UNUSED(x) (void)(x)
#define BITSIZE_OF(x) (sizeof(x) * CHAR_BIT)
#define ASSIGN_PTR(ptr, value) \
    do { \
        if ((ptr) != NULL) { \
            *(ptr) = (value); \
        } \
    } while (0)
#define REQUIRE(condition, value) \
    do { \
        if (!(condition)) { \
            return value; \
        } \
    } while (0)
#define REQUIRE_ELSE(condition, block) \
    do { \
        if (!(condition)) { \
            block; \
        } \
    } while (0)
#define REQUIRE_OK(expr) \
    do { \
        kefir_result_t _expr_result = (expr); \
        if (_expr_result != KEFIR_OK) { \
            return _expr_result; \
        } \
    } while (0)

// Evaluates twice
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

const char *kefir_format(char *buf, kefir_size_t, const char *format, ...);

#endif