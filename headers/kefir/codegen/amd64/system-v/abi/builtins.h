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

#ifndef KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_BUILTINS_H_
#define KEFIR_CODEGEN_AMD64_SYSTEM_V_ABI_BUILTINS_H_

#include "kefir/ir/builtins.h"
#include "kefir/codegen/amd64/system-v/abi/data.h"
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64-sysv.h"

typedef struct kefir_codegen_amd64_sysv_builtin_type {
    kefir_result_t (*layout)(const struct kefir_codegen_amd64_sysv_builtin_type *, const struct kefir_ir_typeentry *,
                             struct kefir_amd64_sysv_data_layout *);
    kefir_result_t (*classify_nested_argument)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                               const struct kefir_ir_typeentry *,
                                               struct kefir_amd64_sysv_parameter_allocation *immediate_allocation,
                                               struct kefir_amd64_sysv_parameter_allocation *allocation);
    kefir_result_t (*classify_immediate_argument)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                                  const struct kefir_ir_typeentry *,
                                                  struct kefir_amd64_sysv_parameter_allocation *);
    kefir_result_t (*allocate_immediate_argument)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                                  const struct kefir_ir_typeentry *,
                                                  struct kefir_amd64_sysv_parameter_location *total_allocation,
                                                  struct kefir_amd64_sysv_parameter_allocation *allocation);
    kefir_result_t (*load_function_argument)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                             const struct kefir_ir_typeentry *, struct kefir_codegen_amd64 *,
                                             struct kefir_amd64_sysv_parameter_allocation *);
    kefir_result_t (*store_function_return)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                            const struct kefir_ir_typeentry *, struct kefir_codegen_amd64 *,
                                            struct kefir_amd64_sysv_parameter_allocation *);
    kefir_result_t (*store_function_argument)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                              const struct kefir_ir_typeentry *, struct kefir_codegen_amd64 *,
                                              struct kefir_amd64_sysv_parameter_allocation *, kefir_size_t);
    kefir_result_t (*load_function_return)(const struct kefir_codegen_amd64_sysv_builtin_type *,
                                           const struct kefir_ir_typeentry *, struct kefir_codegen_amd64 *,
                                           struct kefir_amd64_sysv_parameter_allocation *);
    kefir_result_t (*load_vararg)(struct kefir_mem *, const struct kefir_codegen_amd64_sysv_builtin_type *,
                                  const struct kefir_ir_typeentry *, struct kefir_codegen_amd64 *,
                                  struct kefir_amd64_sysv_function *, const char *,
                                  struct kefir_amd64_sysv_parameter_allocation *);
} kefir_codegen_amd64_sysv_builtin_type_t;

extern const struct kefir_codegen_amd64_sysv_builtin_type KEFIR_CODEGEN_AMD64_SYSV_BUILIN_VARARG_TYPE;
extern const struct kefir_codegen_amd64_sysv_builtin_type *KEFIR_CODEGEN_AMD64_SYSV_BUILTIN_TYPES[];

#endif
