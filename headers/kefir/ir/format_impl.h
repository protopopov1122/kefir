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

#ifndef KEFIR_IR_FORMAT_IMPL_H_
#define KEFIR_IR_FORMAT_IMPL_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"
#include "kefir/util/json.h"

kefir_result_t kefir_ir_format_instr(struct kefir_json_output *, const struct kefir_ir_module *,
                                     const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_none(struct kefir_json_output *, const struct kefir_ir_module *,
                                          const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_i64(struct kefir_json_output *, const struct kefir_ir_module *,
                                         const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_u64(struct kefir_json_output *, const struct kefir_ir_module *,
                                         const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_bool(struct kefir_json_output *, const struct kefir_ir_module *,
                                          const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_u32(struct kefir_json_output *, const struct kefir_ir_module *,
                                         const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_f32(struct kefir_json_output *, const struct kefir_ir_module *,
                                         const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_f64(struct kefir_json_output *, const struct kefir_ir_module *,
                                         const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_typeref(struct kefir_json_output *, const struct kefir_ir_module *,
                                             const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_identifier(struct kefir_json_output *, const struct kefir_ir_module *,
                                                const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_funcref(struct kefir_json_output *, const struct kefir_ir_module *,
                                             const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_coderef(struct kefir_json_output *, const struct kefir_ir_module *,
                                             const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_string(struct kefir_json_output *, const struct kefir_ir_module *,
                                            const struct kefir_irinstr *);

#endif
