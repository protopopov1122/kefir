#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout);

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    kefir_id_t type1_id;
    struct kefir_ir_type *type1 = kefir_ir_module_new_type(mem, &module, 9, &type1_id);
    REQUIRE(type1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_STRUCT, 0, 8));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT8, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_BITS, 0, KEFIR_IR_BITS_PARAM(KEFIR_IR_TYPE_LONG, 38, 0)));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_BITS, 0, KEFIR_IR_BITS_PARAM(KEFIR_IR_TYPE_LONG, 45, 0)));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT8, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_BITS, 0, KEFIR_IR_BITS_PARAM(KEFIR_IR_TYPE_LONG, 51, 0)));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT8, 0, 0));

    struct kefir_ir_type *assign_decl_params = kefir_ir_module_new_type(mem, &module, 2, NULL),
                       *assign_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(assign_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(assign_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *assign_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "assign",
            assign_decl_params, false, assign_decl_result);
    REQUIRE(assign_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *assign = kefir_ir_module_new_function(mem, &module, assign_decl, NULL, 1024);
    REQUIRE(assign != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, assign_decl->name));
    kefir_irbuilder_type_append_v(mem, assign->declaration->params, KEFIR_IR_TYPE_WORD, 0, 0);
    kefir_irbuilder_type_append_v(mem, assign->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);

    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_XCHG, 1);                  // 0: [V, S*]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_PICK, 0);                  // 1: [V, S*, S*]
    kefir_irbuilder_block_appendu32(mem, &assign->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 3);   // 2: [V, S*, B40*]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_PICK, 2);                  // 3: [V, S*, B40*, V]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_STORE40, 0);               // 4: [V, S*]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_PICK, 0);                  // 5: [V, S*, S*]
    kefir_irbuilder_block_appendu32(mem, &assign->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 5);   // 6: [V, S*, B45*]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_PICK, 2);                  // 7: [V, S*, B45*, V]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_IADD1, 1);                 // 8: [V, S*, B45*, V+1]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_STORE48, 0);               // 9: [V, S*]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_PICK, 0);                  // 10: [V, S*, S*]
    kefir_irbuilder_block_appendu32(mem, &assign->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 7);   // 11: [V, S*, B51*]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_PICK, 2);                  // 12: [V, S*, B51*, V]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_IADD1, 2);                 // 13: [V, S*, B51*, V+2]
    kefir_irbuilder_block_appendi64(mem, &assign->body, KEFIR_IROPCODE_STORE56, 0);               // 14: [V, S*]

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}