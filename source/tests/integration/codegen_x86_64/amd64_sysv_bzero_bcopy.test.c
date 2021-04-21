#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));
    codegen.asmgen.settings.enable_comments = false;
    codegen.asmgen.settings.enable_identation = false;

    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_named_function_declaration(mem, &module, "func1", 
            decl_params, false, decl_result, NULL);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl->name, NULL, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);

    kefir_id_t type_id;
    struct kefir_ir_type *type1 = kefir_ir_module_new_type(mem, &module, 0, &type_id);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_BOOL, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BZERO, type_id, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BCOPY, type_id, 0));

    struct kefir_ir_type *type2 = kefir_ir_module_new_type(mem, &module, 0, &type_id);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type2, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BZERO, type_id, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BCOPY, type_id, 0));

    struct kefir_ir_type *type3 = kefir_ir_module_new_type(mem, &module, 0, &type_id);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type3, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BZERO, type_id, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BCOPY, type_id, 0));

    struct kefir_ir_type *type4 = kefir_ir_module_new_type(mem, &module, 0, &type_id);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type4, KEFIR_IR_TYPE_STRUCT, 0, 3));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type4, KEFIR_IR_TYPE_ARRAY, 0, 10));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type4, KEFIR_IR_TYPE_SHORT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type4, KEFIR_IR_TYPE_WORD, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type4, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BZERO, type_id, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BCOPY, type_id, 0));

    struct kefir_ir_type *type5 = kefir_ir_module_new_type(mem, &module, 0, &type_id);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type5, KEFIR_IR_TYPE_UNION, 0, 3));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type5, KEFIR_IR_TYPE_ARRAY, 0, 10));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type5, KEFIR_IR_TYPE_SHORT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type5, KEFIR_IR_TYPE_WORD, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type5, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BZERO, type_id, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BCOPY, type_id, 0));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}