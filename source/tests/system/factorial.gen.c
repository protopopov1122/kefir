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
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "factorial", decl_params, decl_result);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl->identifier, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    kefir_ir_type_append_v(func->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(func->declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PICK, 0);       // 0:  [C; C]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PUSH, 1);       // 1:  [C; C; 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_IGREATER, 0);   // 2:  [C; C > 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_BRANCH, 6);     // 3:  [C]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PUSH, 1);       // 4:  [C; 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_RET, 0);        // 5:  -> 1
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PICK, 0);       // 6:  [C; S]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PICK, 1);       // 7:  [C; S; C]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_IADD1, -1);     // 8:  [C; S; C - 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PICK, 0);       // 9:  [C; S; C - 1; C - 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PUSH, 1);       // 10: [C; S; C - 1; C - 1; 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_IGREATER, 0);   // 11: [C; S; C - 1; C - 1 > 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_BRANCH, 15);    // 12: [C; S; C - 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_POP, 0);        // 13: [C; S]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_RET, 0);        // 14: -> S
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PICK, 0);       // 15: [C; S; C - 1; C - 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_PUT, 3);        // 16: [C - 1; S; C - 1]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_IMUL, 0);       // 17: [C - 1; S * (C - 1)]
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_JMP, 7);        // 18: [C - 1; S * (C - 1)]

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}