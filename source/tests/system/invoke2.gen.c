#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    
    struct kefir_ir_type *proxysum_decl_params = kefir_ir_module_new_type(mem, &module, 3, NULL),
                       *proxysum_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(proxysum_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(proxysum_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *proxysum_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "proxysum", proxysum_decl_params, proxysum_decl_result);
    REQUIRE(proxysum_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *proxysum = kefir_ir_module_new_function(mem, &module, proxysum_decl->identifier, 1024);
    REQUIRE(proxysum != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, proxysum_decl->identifier));

    kefir_ir_module_id_t sumstruct_id;
    struct kefir_ir_type *sumstruct_decl_params = kefir_ir_module_new_type(mem, &module, 4, NULL),
                       *sumstruct_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(sumstruct_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sumstruct_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sumstruct_decl =
        kefir_ir_module_new_function_declaration(mem, &module,
            kefir_ir_module_symbol(mem, &module, "sumstruct", &sumstruct_id), sumstruct_decl_params, sumstruct_decl_result);
    REQUIRE(sumstruct_decl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, sumstruct_decl->identifier));

    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_ir_type_append_v(proxysum_decl_params, KEFIR_IR_TYPE_STRUCT, 0, 1));
    REQUIRE_OK(kefir_ir_type_append_v(proxysum_decl_params, KEFIR_IR_TYPE_ARRAY, 0, 4));
    REQUIRE_OK(kefir_ir_type_append_v(proxysum_decl_params, KEFIR_IR_TYPE_INT64, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(proxysum_decl_result, KEFIR_IR_TYPE_INT64, 0, 0));
    kefir_irblock_append(&proxysum->body, KEFIR_IROPCODE_PUSH, 2);
    kefir_irblock_append(&proxysum->body, KEFIR_IROPCODE_XCHG, 1);
    kefir_irblock_append(&proxysum->body, KEFIR_IROPCODE_INVOKE, sumstruct_id);

    REQUIRE_OK(kefir_ir_type_append_v(sumstruct_decl_params, KEFIR_IR_TYPE_INT64, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(sumstruct_decl_params, KEFIR_IR_TYPE_STRUCT, 0, 1));
    REQUIRE_OK(kefir_ir_type_append_v(sumstruct_decl_params, KEFIR_IR_TYPE_ARRAY, 0, 4));
    REQUIRE_OK(kefir_ir_type_append_v(sumstruct_decl_params, KEFIR_IR_TYPE_INT64, 0, 0));
    REQUIRE_OK(kefir_ir_type_append_v(sumstruct_decl_result, KEFIR_IR_TYPE_INT64, 0, 0));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}