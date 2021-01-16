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

    kefir_ir_module_id_t type1_id;
    struct kefir_ir_type *type1 = kefir_ir_module_new_type(mem, &module, 9, &type1_id);
    REQUIRE(type1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_STRUCT, 0, 8));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT8, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT8, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT32, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT32, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT64, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(type1, KEFIR_IR_TYPE_INT64, 0, 0));

    struct kefir_ir_type *sum_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *sum_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(sum_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sum_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sum_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "sum", NULL, sum_decl_params, false, sum_decl_result);
    REQUIRE(sum_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *sum = kefir_ir_module_new_function(mem, &module, sum_decl->identifier, NULL, 1024);
    REQUIRE(sum != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, sum_decl->identifier));
    kefir_ir_type_append_v(sum->declaration->params, KEFIR_IR_TYPE_WORD, 0, 0);
    kefir_ir_type_append_v(sum->declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);

    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PUSH, 0);                 // 0: [S*, 0]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, 0, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 1); // 2: [S*, 0, U8*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_LOAD8U, 0);               // 3: [S*, 0, U8]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, U8]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, U8, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 2); // 2: [S*, U8, I8*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_LOAD8I, 0);               // 3: [S*, U8, I8]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, U8+I8]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, U8+I8, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 3); // 2: [S*, U8+I8, U16*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_LOAD16U, 0);              // 3: [S*, U8+I8, U16]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, U8+I8+U16]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, U8+I8+U16, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 4); // 2: [S*, U8+I8+U16, I16*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_LOAD16I, 0);              // 3: [S*, U8+I8+U16, I16]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, U8+I8+U16+I16]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, U8+I8+U16+I16, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 5); // 2: [S*, U8+I8+U16+I16, U32*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_LOAD32U, 0);              // 3: [S*, U8+I8+U16+I16, U32]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, U8+I8+U16+I16+U32]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, U8+I8+U16+I16+U32, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 6); // 2: [S*, U8+I8+U16+I16+U32, I32*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_LOAD32I, 0);              // 3: [S*, U8+I8+U16+I16+U32, I32]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, U8+I8+U16+I16+U32+I32]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, U8+I8+U16+I16+U32+I32, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 7); // 2: [S*, U8+I8+U16+I16+U32+I32, I64*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_LOAD64, 0);               // 3: [S*, U8+I8+U16+I16+U32+I32, I64]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, U8+I8+U16+I16+U32+I32+I64]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, SUM, S*]
    kefir_irblock_append2(&sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 8); // 2: [S*, VAL, SUM*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_XCHG, 1);                 // 1: [S*, SUM*, VAL]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, SUM*, VAL, SUM*]
    kefir_irblock_append(&sum->body, KEFIR_IROPCODE_STORE64, 0);              // 1: [S*, SUM*]

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}