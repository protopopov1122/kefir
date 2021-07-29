/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/core/vector.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/ir/type.h"

typedef enum kefir_amd64_sysv_data_class {
    KEFIR_AMD64_SYSV_PARAM_INTEGER = 0,
    KEFIR_AMD64_SYSV_PARAM_SSE,
    KEFIR_AMD64_SYSV_PARAM_SSEUP,
    KEFIR_AMD64_SYSV_PARAM_X87,
    KEFIR_AMD64_SYSV_PARAM_X87UP,
    KEFIR_AMD64_SYSV_PARAM_COMPLEX_X87,
    KEFIR_AMD64_SYSV_PARAM_NO_CLASS,
    KEFIR_AMD64_SYSV_PARAM_MEMORY
} kefir_amd64_sysv_data_class_t;

#define KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "AMD64 System-V ABI: "

#endif
