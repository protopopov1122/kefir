#ifndef KEFIR_IR_FORMAT_H_
#define KEFIR_IR_FORMAT_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"

kefir_result_t kefir_ir_format_module(FILE *, const struct kefir_ir_module *);
kefir_result_t kefir_ir_format_type(FILE *, struct kefir_ir_type *);

#endif