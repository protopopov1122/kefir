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

#include "kefir/parser/context.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_context_default(struct kefir_parser_context *context) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to parser context"));

    context->integer_max_value = KEFIR_INT_MAX;
    context->uinteger_max_value = KEFIR_UINT_MAX;
    context->long_max_value = KEFIR_LONG_MAX;
    context->ulong_max_value = KEFIR_ULONG_MAX;
    context->long_long_max_value = KEFIR_LONG_LONG_MAX;
    context->ulong_long_max_value = KEFIR_ULONG_LONG_MAX;
    context->newline = U'\n';
    return KEFIR_OK;
}
