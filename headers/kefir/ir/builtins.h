#ifndef KEFIR_IR_BUILTINS_H_
#define KEFIR_IR_BUILTINS_H_

#include "kefir/core/basic-types.h"

typedef enum kefir_ir_builtin_type {
    KEFIR_IR_TYPE_BUILTIN_VARARG = 0,
    // Misc
    KEFIR_IR_TYPE_BUILTIN_COUNT
} kefir_ir_builtin_type_t;

#endif