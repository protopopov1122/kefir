/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

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

#ifndef KEFIR_CORE_UTIL_H_
#define KEFIR_CORE_UTIL_H_

#include <limits.h>
#include "kefir/core/basic-types.h"

#define UNUSED(x) (void) (x)
#define UNOWNED(x) x
#define BITSIZE_OF(x) (sizeof(x) * CHAR_BIT)
#define ASSIGN_CAST(type, dest, expr) \
    do {                              \
        *(dest) = ((type) (expr));    \
    } while (0)
#define ASSIGN_DECL_CAST(type, ident, expr) type ident = ((type) (expr));
#define ASSIGN_PTR(ptr, value) \
    do {                       \
        if ((ptr) != NULL) {   \
            *(ptr) = (value);  \
        }                      \
    } while (0)

#define REQUIRE(condition, value) \
    do {                          \
        if (!(condition)) {       \
            return value;         \
        }                         \
    } while (0)
#define REQUIRE_ELSE(condition, block) \
    do {                               \
        if (!(condition)) {            \
            block;                     \
        }                              \
    } while (0)
#define REQUIRE_CHAIN(result, expr)  \
    do {                             \
        if (*(result) == KEFIR_OK) { \
            *(result) = (expr);      \
        }                            \
    } while (0)
#define REQUIRE_OK(expr)                      \
    do {                                      \
        kefir_result_t _expr_result = (expr); \
        if (_expr_result != KEFIR_OK) {       \
            return _expr_result;              \
        }                                     \
    } while (0)
#define REQUIRE_YIELD(expr, defReturn)            \
    do {                                          \
        kefir_result_t _expr_result = (expr);     \
        if (_expr_result == KEFIR_OK) {           \
            return defReturn;                     \
        } else if (_expr_result != KEFIR_YIELD) { \
            return _expr_result;                  \
        }                                         \
    } while (0)
#define REQUIRE_CHAIN_SET(result, condition, expr)   \
    do {                                             \
        if (*(result) == KEFIR_OK && !(condition)) { \
            *(result) = (expr);                      \
        }                                            \
    } while (0)

// Evaluates twice
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

const char *kefir_format(char *buf, kefir_size_t, const char *format, ...);

#endif
