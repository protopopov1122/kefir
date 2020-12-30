#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);

    struct kefir_ir_type decl_params, decl_result;
    struct kefir_ir_function_decl decl;
    struct kefir_ir_function func;
    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_params));
    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_result));
    kefir_ir_function_decl_alloc(mem, "trunc1", &decl_params, &decl_result, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func.declaration->result, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_TRUNCATE1, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_result));
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_params));

    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_params));
    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_result));
    kefir_ir_function_decl_alloc(mem, "extend8", &decl_params, &decl_result, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func.declaration->result, KEFIR_IR_TYPE_CHAR, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_EXTEND8, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_result));
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_params));

    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_params));
    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_result));
    kefir_ir_function_decl_alloc(mem, "extend16", &decl_params, &decl_result, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func.declaration->result, KEFIR_IR_TYPE_SHORT, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_EXTEND16, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_result));
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_params));

    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_params));
    REQUIRE_OK(kefir_ir_type_alloc(mem, 1, &decl_result));
    kefir_ir_function_decl_alloc(mem, "extend32", &decl_params, &decl_result, &decl);
    kefir_ir_function_alloc(mem, &decl, 1024, &func);
    kefir_ir_type_append_v(func.declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func.declaration->result, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_EXTEND32, 0);
    kefir_irblock_append(&func.body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &func);
    kefir_ir_function_free(mem, &func);
    kefir_ir_function_decl_free(mem, &decl);
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_result));
    REQUIRE_OK(kefir_ir_type_free(mem, &decl_params));

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    return EXIT_SUCCESS;
}