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
    kefir_ir_function_t func;
    kefir_ir_function_decl_alloc(mem, "bool_and", 3, 1, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(&func.declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(&func.declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(&func.declaration->result, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_BAND, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);

    kefir_ir_function_decl_alloc(mem, "bool_or", 3, 1, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(&func.declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(&func.declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(&func.declaration->result, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_BOR, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);

    kefir_ir_function_decl_alloc(mem, "bool_not", 3, 1, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(&func.declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(&func.declaration->result, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_BNOT, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    return EXIT_SUCCESS;
}