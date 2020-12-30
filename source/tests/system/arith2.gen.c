#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_init(&module));
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 3),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 1);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl decl;
    struct kefir_ir_function func;
    kefir_ir_function_decl_alloc(mem, "arith2", decl_params, decl_result, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func.declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_INOT, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 3);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_ILSHIFT, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 5);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IXOR, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IOR, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_PUSH, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IARSHIFT, 2);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_IAND, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}