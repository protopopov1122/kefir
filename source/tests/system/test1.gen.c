#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);

    kefir_irfunction_t func;
    kefir_irfunction_alloc(mem, "test1", 2, 1, 1024, &func);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_ir_type_append_v(&func.declaration.params, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_ir_type_append_v(&func.declaration.result, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 10);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IADD, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IADD, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    kefir_irfunction_free(mem, &func);
    return EXIT_SUCCESS;
}