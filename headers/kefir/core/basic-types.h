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

#ifndef KEFIR_CORE_BASIC_TYPES_H_
#define KEFIR_CORE_BASIC_TYPES_H_

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>
#include <uchar.h>
#include "kefir/core/base.h"

typedef bool kefir_bool_t;
typedef char kefir_char_t;
#define KEFIR_CHAR_MIN CHAR_MIN
#define KEFIR_CHAR_MAX CHAR_MAX

typedef char32_t kefir_char32_t;
#define KEFIR_CHAR32_MIN 0
#define KEFIR_CHAR32_MAX UINT_LEAST32_MIN

typedef char16_t kefir_char16_t;
#define KEFIR_CHAR16_MIN 0
#define KEFIR_CHAR16_MAX UINT_LEAST16_MIN

typedef wchar_t kefir_wchar_t;
#define KEFIR_WCHAR_MIN WCHAR_MIN
#define KEFIR_WCHAR_MAX WCHAR_MAX

typedef int kefir_int_t;
typedef unsigned int kefir_uint_t;
#define KEFIR_INT_MIN INT_MIN
#define KEFIR_INT_MAX INT_MAX
#define KEFIR_INT_FMT "%i"
#define KEFIR_UINT_MIN 0
#define KEFIR_UINT_MAX UINT_MAX
#define KEFIR_UINT_FMT "%u"

typedef long kefir_long_t;
typedef unsigned long kefir_ulong_t;
#define KEFIR_LONG_MIN LONG_MIN
#define KEFIR_LONG_MAX LONG_MAX
#define KEFIR_LONG_FMT "%li"
#define KEFIR_ULONG_MIN 0
#define KEFIR_ULONG_MAX ULONG_MAX
#define KEFIR_ULONG_FMT "%lu"

typedef long long kefir_long_long_t;
typedef unsigned long long kefir_ulong_long_t;
#define KEFIR_LONG_LONG_MIN LLONG_MIN
#define KEFIR_LONG_LONG_MAX LLONG_MAX
#define KEFIR_LONG_LONG_FMT "%lli"
#define KEFIR_ULONG_LONG_MIN 0
#define KEFIR_ULONG_LONG_MAX ULONG_MAX
#define KEFIR_ULONG_LONG_FMT "%llu"

typedef int8_t kefir_int8_t;
typedef uint8_t kefir_uint8_t;
#define KEFIR_INT8_MIN INT8_MIN
#define KEFIR_INT8_MAX INT8_MAX
#define KEFIR_INT8_FMT "%" PRId8
#define KEFIR_UINT8_MIN 0
#define KEFIR_UINT8_MAX UINT8_MAX
#define KEFIR_UINT8_FMT "%" PRIu8

typedef int16_t kefir_int16_t;
typedef uint16_t kefir_uint16_t;
#define KEFIR_INT16_MIN INT16_MIN
#define KEFIR_INT16_MAX INT16_MAX
#define KEFIR_INT16_FMT "%" PRId16
#define KEFIR_UINT16_MIN 0
#define KEFIR_UINT16_MAX UINT16_MAX
#define KEFIR_UINT16_FMT "%" PRIu16

typedef int32_t kefir_int32_t;
typedef uint32_t kefir_uint32_t;
#define KEFIR_INT32_MIN INT32_MIN
#define KEFIR_INT32_MAX INT32_MAX
#define KEFIR_INT32_FMT "%" PRId32
#define KEFIR_UINT32_MIN 0
#define KEFIR_UINT32_MAX UINT32_MAX
#define KEFIR_UINT32_FMT "%" PRIu32

typedef int64_t kefir_int64_t;
typedef uint64_t kefir_uint64_t;
#define KEFIR_INT64_MIN INT64_MIN
#define KEFIR_INT64_MAX INT64_MAX
#define KEFIR_INT64_FMT "%" PRId64
#define KEFIR_UINT64_MIN 0
#define KEFIR_UINT64_MAX UINT64_MAX
#define KEFIR_UINT64_FMT "%" PRIu64

typedef size_t kefir_size_t;
typedef uintptr_t kefir_uptr_t;
typedef float kefir_float32_t;
typedef double kefir_float64_t;
typedef kefir_uint32_t kefir_id_t;

#define KEFIR_ID_FMT KEFIR_UINT32_FMT

#define KEFIR_FLOAT32_FMT "%f"
#define KEFIR_FLOAT64_FMT "%lf"

#define KEFIR_SIZE_MIN SIZE_MIN
#define KEFIR_SIZE_MAX SIZE_MAX
#define KEFIR_SIZE_FMT "%zu"

typedef enum kefir_result {
    // Normal
    KEFIR_OK = 0,
    KEFIR_YIELD,
    // Warning
    KEFIR_ITERATOR_END,
    KEFIR_NO_MATCH,
    KEFIR_NOT_FOUND,
    KEFIR_ALREADY_EXISTS,
    // Generic error
    KEFIR_UNKNOWN_ERROR,
    KEFIR_INTERNAL_ERROR,
    KEFIR_OS_ERROR,
    KEFIR_MALFORMED_ARG,
    KEFIR_OUT_OF_BOUNDS,
    KEFIR_MEMALLOC_FAILURE,
    KEFIR_NOT_SUPPORTED,
    KEFIR_NOT_IMPLEMENTED,
    KEFIR_OUT_OF_SPACE,
    // Language-specific error
    KEFIR_SYNTAX_ERROR,
    KEFIR_NOT_CONSTANT,
    KEFIR_STATIC_ASSERT
} kefir_result_t;

typedef enum kefir_result_category {
    KEFIR_RESULT_CATEGORY_UNKNOWN,
    KEFIR_RESULT_CATEGORY_NORMAL,
    KEFIR_RESULT_CATEGORY_WARNING,
    KEFIR_RESULT_CATEGORY_GENERIC_ERROR,
    KEFIR_RESULT_CATEGORY_LANGUAGE_ERROR
} kefir_result_category_t;

kefir_result_category_t kefir_result_get_category(kefir_result_t);

#endif
