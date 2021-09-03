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

#ifndef KEFIR_CORE_SOURCE_LOCATION_H_
#define KEFIR_CORE_SOURCE_LOCATION_H_

#include "kefir/core/basic-types.h"

typedef kefir_uint_t kefir_source_location_line_t;
typedef kefir_uint_t kefir_source_location_column_t;

typedef struct kefir_source_location {
    const char *source;
    kefir_source_location_line_t line;
    kefir_source_location_column_t column;
} kefir_source_location_t;

kefir_result_t kefir_source_location_empty(struct kefir_source_location *);
kefir_result_t kefir_source_location_init(struct kefir_source_location *, const char *, kefir_source_location_line_t,
                                          kefir_source_location_column_t);
kefir_bool_t kefir_source_location_get(const struct kefir_source_location *, const char **,
                                       kefir_source_location_line_t *, kefir_source_location_column_t *);

#endif
