#ifndef KEFIR_IR_FORMAT_IMPL_H_
#define KEFIR_IR_FORMAT_IMPL_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"

kefir_result_t kefir_ir_format_instr(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_none(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_i64(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_u64(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_f32(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_f64(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_typeref(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_dataref(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_funcref(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);
kefir_result_t kefir_ir_format_instr_coderef(FILE *, const struct kefir_ir_module *, const struct kefir_irinstr *);

#endif