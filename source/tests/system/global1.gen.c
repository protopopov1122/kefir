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
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    kefir_ir_module_id_t type1_id;
    struct kefir_ir_type *type1 = kefir_ir_module_new_type(mem, &module, 5, &type1_id);
    REQUIRE(type1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_STRUCT, 0, 4));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT8, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT64, 0, 0));

    kefir_ir_module_id_t global1;
    REQUIRE(kefir_ir_module_symbol(mem, &module, "global1", &global1) != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, "global1"));

    struct kefir_ir_type *fill_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *fill_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(fill_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(fill_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *fill_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "fill", NULL, fill_decl_params, false, fill_decl_result);
    REQUIRE(fill_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *fill = kefir_ir_module_new_function(mem, &module, fill_decl->identifier, NULL, 1024);
    REQUIRE(fill != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, fill_decl->identifier));
    kefir_irbuilder_type_append_v(mem, fill->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);

    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_GETGLOBAL, global1);           // 0:  [VAL, S*]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_XCHG, 1);                      // 1:  [S*, VAL]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_PICK, 0);                      // 1:  [S*, VAL, VAL]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_PICK, 2);                      // 2:  [S*, VAL, VAL, S*]
    kefir_irbuilder_block_append2(mem, &fill->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 1);      // 3:  [S*, VAL, VAL, U8*]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_STORE8, 0);                    // 4:  [S*, VAL]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_PICK, 0);                      // 5:  [S*, VAL, VAL]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_PICK, 2);                      // 6:  [S*, VAL, VAL, S*]
    kefir_irbuilder_block_append2(mem, &fill->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 2);      // 7:  [S*, VAL, VAL, U16*]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_STORE16, 0);                   // 8:  [S*, VAL]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_PICK, 0);                      // 9:  [S*, VAL, VAL]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_PICK, 2);                      // 10: [S*, VAL, VAL, S*]
    kefir_irbuilder_block_append2(mem, &fill->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 3);      // 11: [S*, VAL, VAL, U32*]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_STORE32, 0);                   // 12: [S*, VAL]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_PICK, 1);                      // 13: [S*, VAL, S*]
    kefir_irbuilder_block_append2(mem, &fill->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 4);      // 14: [S*, VAL, U64*]
    kefir_irbuilder_block_append(mem, &fill->body, KEFIR_IROPCODE_STORE64, 0);                   // 15: [S*]

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}