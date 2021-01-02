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
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 2, NULL),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "bool_and", decl_params, decl_result);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl->identifier, 2);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    kefir_ir_type_append_v(func->declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(func->declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(func->declaration->result, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_BAND, 0);
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_RET, 0);

    decl_params = kefir_ir_module_new_type(mem, &module, 2, NULL),
    decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    decl = kefir_ir_module_new_function_declaration(mem, &module, "bool_or", decl_params, decl_result);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    func = kefir_ir_module_new_function(mem, &module, decl->identifier, 2);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    kefir_ir_type_append_v(func->declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(func->declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(func->declaration->result, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_BOR, 0);
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_RET, 0);

    decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
    decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    decl = kefir_ir_module_new_function_declaration(mem, &module, "bool_not", decl_params, decl_result);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    func = kefir_ir_module_new_function(mem, &module, decl->identifier, 2);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    kefir_ir_type_append_v(func->declaration->params, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_ir_type_append_v(func->declaration->result, KEFIR_IR_TYPE_BOOL, 0, 0);
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_BNOT, 0);
    kefir_irblock_append(&func->body, KEFIR_IROPCODE_RET, 0);
    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module);

    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    return EXIT_SUCCESS;
}