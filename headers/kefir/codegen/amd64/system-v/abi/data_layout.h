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

#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_LAYOUT_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_DATA_LAYOUT_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"
#include "kefir/ir/type.h"

typedef struct kefir_amd64_sysv_data_layout {
    kefir_size_t size;
    kefir_size_t alignment;
    bool aligned;
    kefir_size_t relative_offset;
} kefir_amd64_sysv_data_layout_t;

kefir_result_t kefir_amd64_sysv_type_layout_of(struct kefir_mem *, const struct kefir_ir_type *, kefir_size_t,
                                               kefir_size_t, struct kefir_vector *);

kefir_result_t kefir_amd64_sysv_type_layout(const struct kefir_ir_type *, struct kefir_mem *, struct kefir_vector *);

#endif
