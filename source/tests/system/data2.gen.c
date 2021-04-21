#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout);

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    kefir_id_t type1_id;
    struct kefir_ir_type *type1 = kefir_ir_module_new_type(mem, &module, 10, &type1_id);
    REQUIRE(type1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_STRUCT, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_STRUCT, 0, 3));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_ARRAY, 0, 10));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_UNION, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_ARRAY, 0, 8));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_LONG, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT64, 0, 0));

    struct kefir_ir_type *arrptr_decl_params = kefir_ir_module_new_type(mem, &module, 2, NULL),
                       *arrptr_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(arrptr_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(arrptr_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *arrptr_decl =
        kefir_ir_module_new_named_function_declaration(mem, &module, "arrptr",
            arrptr_decl_params, false, arrptr_decl_result, NULL);
    REQUIRE(arrptr_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *arrptr = kefir_ir_module_new_function(mem, &module, arrptr_decl->name, NULL, 5);
    REQUIRE(arrptr != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, arrptr_decl->name));
    kefir_irbuilder_type_append_v(mem, arrptr->declaration->params, KEFIR_IR_TYPE_WORD, 0, 0);
    kefir_irbuilder_type_append_v(mem, arrptr->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, arrptr->declaration->result, KEFIR_IR_TYPE_WORD, 0, 0);

    kefir_irbuilder_block_appendi64(mem, &arrptr->body, KEFIR_IROPCODE_XCHG, 1);
    kefir_irbuilder_block_appendu32(mem, &arrptr->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 2);
    kefir_irbuilder_block_appendu32(mem, &arrptr->body, KEFIR_IROPCODE_OFFSETPTR, type1_id, 4);
    kefir_irbuilder_block_appendi64(mem, &arrptr->body, KEFIR_IROPCODE_XCHG, 1);
    kefir_irbuilder_block_appendu32(mem, &arrptr->body, KEFIR_IROPCODE_ELEMENTPTR, type1_id, 8);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}