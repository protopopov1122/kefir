#ifndef KEFIR_CODEGEN_CODEGEN_H_
#define KEFIR_CODEGEN_CODEGEN_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/ir/module.h"

typedef struct kefir_codegen {
    kefir_result_t (*translate)(struct kefir_codegen *, const struct kefir_ir_module *);
    kefir_result_t (*close)(struct kefir_codegen *);

    void *data;
} kefir_codegen_t;

#define KEFIR_CODEGEN_TRANSLATE(codegen, module) \
    ((codegen)->translate((codegen), (module)))
#define KEFIR_CODEGEN_CLOSE(codegen) \
    ((codegen)->close((codegen)))

#endif