#include <stdlib.h>
#include <stdio.h>
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
    struct kefir_codegen_amd64_sysv_module sysv_module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_alloc(mem, &sysv_module, &module));
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_named_function_declaration(mem, &module, "func1", 
            decl_params, false, decl_result, NULL);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl->identifier, NULL, 0);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->identifier));
    struct kefir_amd64_sysv_function sysv_func;
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));
    codegen.asmgen.settings.enable_comments = false;
    codegen.asmgen.settings.enable_identation = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_INT64, 0, 0));

    REQUIRE_OK(kefir_amd64_sysv_function_alloc(mem, &sysv_module, func, &sysv_func));
    REQUIRE_OK(kefir_amd64_sysv_function_epilogue(&codegen, &sysv_func));
    REQUIRE_OK(kefir_amd64_sysv_function_free(mem, &sysv_func));

    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_free(mem, &sysv_module));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}