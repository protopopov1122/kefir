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

    struct kefir_ir_type *extract_decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                       *extract_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(extract_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(extract_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *extract_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "insert", 
            extract_decl_params, false, extract_decl_result);
    REQUIRE(extract_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *extract_func = kefir_ir_module_new_function(mem, &module, extract_decl, NULL, 1024);
    REQUIRE(extract_func != NULL, KEFIR_INTERNAL_ERROR);
    
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 0, 10));
    
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 1, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 2, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 4, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 8, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 16, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 32, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 1, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 2, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 4, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 8, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extract_func->body, KEFIR_IROPCODE_EXTRACTBITS, 16, 10));


    struct kefir_ir_type *insert_decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                       *insert_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(insert_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(insert_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *insert_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "extract", 
            insert_decl_params, false, insert_decl_result);
    REQUIRE(insert_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *insert_func = kefir_ir_module_new_function(mem, &module, insert_decl, NULL, 1024);
    REQUIRE(insert_func != NULL, KEFIR_INTERNAL_ERROR);
    
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 10));
    
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 1, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 2, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 4, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 8, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 16, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 32, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 1, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 2, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 4, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 8, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 16, 10));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}