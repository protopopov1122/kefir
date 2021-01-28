#ifndef KEFIR_CORE_BASIC_TYPES_H_
#define KEFIR_CORE_BASIC_TYPES_H_

#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include "kefir/core/base.h"

typedef int32_t kefir_int32_t;
typedef uint32_t kefir_uint32_t;
typedef int64_t kefir_int64_t;
typedef uint64_t kefir_uint64_t;
typedef size_t kefir_size_t;
typedef uintptr_t kefir_uptr_t;
typedef float kefir_float32_t;
typedef double kefir_float64_t;
typedef kefir_uint32_t kefir_id_t;

#define KEFIR_INT32_MIN INT32_MIN
#define KEFIR_INT32_MAX INT32_MAX
#define KEFIR_INT32_FMT "%"PRId32
#define KEFIR_UINT32_MIN UINT32_MAX
#define KEFIR_UINT32_MAX UINT32_MAX
#define KEFIR_UINT32_FMT "%"PRIu32
#define KEFIR_INT64_MIN INT64_MIN
#define KEFIR_INT64_MAX INT64_MAX
#define KEFIR_INT64_FMT "%"PRId64
#define KEFIR_UINT64_MIN UINT64_MAX
#define KEFIR_UINT64_MAX UINT64_MAX
#define KEFIR_UINT64_FMT "%"PRIu64
#define KEFIR_SIZE_MIN SIZE_MIN
#define KEFIR_SIZE_MAX SIZE_MAX
#define KEFIR_SIZE_FMT "%zu"

typedef enum kefir_result {
    KEFIR_OK = 0,
    KEFIR_YIELD,
    KEFIR_UNKNOWN_ERROR,
    KEFIR_INTERNAL_ERROR,
    KEFIR_MALFORMED_ARG,
    KEFIR_OUT_OF_BOUNDS,
    KEFIR_MEMALLOC_FAILURE,
    KEFIR_NOT_SUPPORTED,
    KEFIR_NOT_IMPLEMENTED,
    KEFIR_NOT_FOUND,
    KEFIR_ALREADY_EXISTS
} kefir_result_t;

#endif