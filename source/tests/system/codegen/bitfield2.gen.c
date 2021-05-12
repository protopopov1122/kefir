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
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_BITS, 0, KEFIR_IR_BITS_PARAM(KEFIR_IR_TYPE_INT64, 20)));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_BITS, 0, KEFIR_IR_BITS_PARAM(KEFIR_IR_TYPE_INT64, 30)));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT8, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_BITS, 0, KEFIR_IR_BITS_PARAM(KEFIR_IR_TYPE_INT64, 40)));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT8, 0, 0));

    struct kefir_ir_type *sum_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *sum_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(sum_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sum_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sum_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "sum",
            sum_decl_params, false, sum_decl_result);
    REQUIRE(sum_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *sum = kefir_ir_module_new_function(mem, &module, sum_decl, NULL, 1024);
    REQUIRE(sum != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, sum_decl->name));
    kefir_irbuilder_type_append_v(mem, sum->declaration->params, KEFIR_IR_TYPE_WORD, 0, 0);
    kefir_irbuilder_type_append_v(mem, sum->declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);

    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_PUSHI64, 0);              // 0: [S*, 0]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_PICK, 1);                 // 1: [S*, 0, S*]
    kefir_irbuilder_block_appendu32(mem, &sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 2);  // 2: [S*, 0, B20*]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_LOAD24U, 0);                // 3: [S*, 0, B20]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_IADD, 0);                 // 4: [S*, B20]
    kefir_irbuilder_block_appendu32(mem, &sum->body, KEFIR_IROPCODE_EXTSBITS, 0, 20);         // 5: [S*, V1]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_PICK, 1);                 // 6: [S*, V1, S*]
    kefir_irbuilder_block_appendu32(mem, &sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 4);  // 7: [S*, V1, B30*]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_LOAD32U, 0);                // 8: [S*, V1, B30]
    kefir_irbuilder_block_appendu32(mem, &sum->body, KEFIR_IROPCODE_EXTSBITS, 0, 30);         // 9: [S*, V1, V2]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_IADD, 0);                 // 10: [S*, V1+V2]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_PICK, 1);                 // 11: [S*, V1+V2, S*]
    kefir_irbuilder_block_appendu32(mem, &sum->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 7);  // 12: [S*, V1+V2, B40*]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_LOAD40U, 0);                // 13: [S*, V1+V2, B40]
    kefir_irbuilder_block_appendu32(mem, &sum->body, KEFIR_IROPCODE_EXTSBITS, 0, 40);         // 14: [S*, V1+V2, V3]
    kefir_irbuilder_block_appendi64(mem, &sum->body, KEFIR_IROPCODE_IADD, 0);                 // 15: [S*, V1+V2+V3]

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}