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

#ifndef KEFIR_UTIL_JSON_H_
#define KEFIR_UTIL_JSON_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"

#define KEFIR_JSON_MAX_DEPTH 1024

typedef enum kefir_json_state {
    KEFIR_JSON_STATE_INIT = 0,
    KEFIR_JSON_STATE_OBJECT_EMPTY,
    KEFIR_JSON_STATE_OBJECT_FIELD,
    KEFIR_JSON_STATE_OBJECT,
    KEFIR_JSON_STATE_ARRAY_EMPTY,
    KEFIR_JSON_STATE_ARRAY,
} kefir_json_state_t;

typedef struct kefir_json_output {
    FILE *file;
    kefir_size_t indent;
    kefir_json_state_t state[KEFIR_JSON_MAX_DEPTH];
    kefir_size_t level;
} kefir_json_output_t;

kefir_result_t kefir_json_output_init(struct kefir_json_output *, FILE *, kefir_size_t);
kefir_result_t kefir_json_output_finalize(struct kefir_json_output *);

kefir_result_t kefir_json_output_object_begin(struct kefir_json_output *);
kefir_result_t kefir_json_output_object_end(struct kefir_json_output *);
kefir_result_t kefir_json_output_array_begin(struct kefir_json_output *);
kefir_result_t kefir_json_output_array_end(struct kefir_json_output *);

kefir_result_t kefir_json_output_object_key(struct kefir_json_output *, const char *);
kefir_result_t kefir_json_output_integer(struct kefir_json_output *, kefir_int64_t);
kefir_result_t kefir_json_output_uinteger(struct kefir_json_output *, kefir_uint64_t);
kefir_result_t kefir_json_output_float(struct kefir_json_output *, kefir_float64_t);
kefir_result_t kefir_json_output_long_double(struct kefir_json_output *, kefir_long_double_t);
kefir_result_t kefir_json_output_hexfloat(struct kefir_json_output *, kefir_float64_t);
kefir_result_t kefir_json_output_string(struct kefir_json_output *, const char *);
kefir_result_t kefir_json_output_raw_string(struct kefir_json_output *, const char *, kefir_size_t);
kefir_result_t kefir_json_output_boolean(struct kefir_json_output *, kefir_bool_t);
kefir_result_t kefir_json_output_null(struct kefir_json_output *);

#endif
