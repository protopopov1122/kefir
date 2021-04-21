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
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_named_function_declaration(mem, &module, "fillzeros",
            decl_params, false, decl_result, NULL);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl->name, NULL, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name));

    kefir_id_t type_id;
    struct kefir_ir_type *type1 = kefir_ir_module_new_type(mem, &module, 0, &type_id);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_ARRAY, 0, 8));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, type1, KEFIR_IR_TYPE_INT, 0, 0));

    kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_WORD, 0, 0);
    kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_BZERO, type_id, 0);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}