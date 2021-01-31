#ifndef KEFIR_IR_PRINT_H_
#define KEFIR_IR_PRINT_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"

kefir_result_t kefir_ir_print_module(FILE *, const struct kefir_ir_module *);

#endif