#ifndef KEFIR_IR_FORMAT_H_
#define KEFIR_IR_FORMAT_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"
#include "kefir/util/json.h"

kefir_result_t kefir_ir_format_module(FILE *, const struct kefir_ir_module *);
kefir_result_t kefir_ir_format_module_json(struct kefir_json_output *, const struct kefir_ir_module *);
kefir_result_t kefir_ir_format_type(FILE *, const struct kefir_ir_type *);
kefir_result_t kefir_ir_format_type_json(struct kefir_json_output *, const struct kefir_ir_type *);

#endif
