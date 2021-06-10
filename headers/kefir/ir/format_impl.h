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
