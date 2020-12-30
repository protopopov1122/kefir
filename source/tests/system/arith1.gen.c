#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);

    struct kefir_ir_function_decl decl;
    struct kefir_ir_function func;
    kefir_ir_function_decl_alloc(mem, "arith1", 2, 1, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(&func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(&func.declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 7);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IADD, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IADD, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IADD1, 5);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_ISUB, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 9);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 3);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IDIV, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IMUL, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 157);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IMOD, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INEG, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);
    return EXIT_SUCCESS;
}