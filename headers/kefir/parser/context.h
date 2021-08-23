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

#ifndef KEFIR_PARSER_CONTEXT_H_
#define KEFIR_PARSER_CONTEXT_H_

#include "kefir/parser/base.h"
#include "kefir/core/basic-types.h"

typedef struct kefir_parser_integral_types {
    kefir_uint64_t integer_max_value;
    kefir_uint64_t uinteger_max_value;
    kefir_uint64_t long_max_value;
    kefir_uint64_t ulong_max_value;
    kefir_uint64_t long_long_max_value;
    kefir_uint64_t ulong_long_max_value;
} kefir_parser_integral_types_t;

kefir_result_t kefir_parser_integral_types_default(struct kefir_parser_integral_types *);

#endif
