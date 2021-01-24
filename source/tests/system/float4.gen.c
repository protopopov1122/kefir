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

    struct kefir_ir_type *floatToLong_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *floatToLong_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(floatToLong_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(floatToLong_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *floatToLong_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "floatToLong", NULL, floatToLong_decl_params, false, floatToLong_decl_result);
    REQUIRE(floatToLong_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *floatToLong = kefir_ir_module_new_function(mem, &module, floatToLong_decl->identifier, NULL, 1024);
    REQUIRE(floatToLong != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, floatToLong_decl->identifier));
    kefir_ir_type_append_v(floatToLong->declaration->params, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_ir_type_append_v(floatToLong->declaration->result, KEFIR_IR_TYPE_LONG, 0, 3);
    kefir_irblock_append(&floatToLong->body, KEFIR_IROPCODE_F32CINT, 0);

    struct kefir_ir_type *doubleToLong_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *doubleToLong_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(doubleToLong_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(doubleToLong_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *doubleToLong_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "doubleToLong", NULL, doubleToLong_decl_params, false, doubleToLong_decl_result);
    REQUIRE(doubleToLong_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *doubleToLong = kefir_ir_module_new_function(mem, &module, doubleToLong_decl->identifier, NULL, 1024);
    REQUIRE(doubleToLong != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, doubleToLong_decl->identifier));
    kefir_ir_type_append_v(doubleToLong->declaration->params, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_ir_type_append_v(doubleToLong->declaration->result, KEFIR_IR_TYPE_LONG, 0, 3);
    kefir_irblock_append(&doubleToLong->body, KEFIR_IROPCODE_F64CINT, 0);

    struct kefir_ir_type *longToFloat_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *longToFloat_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(longToFloat_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(longToFloat_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *longToFloat_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "longToFloat", NULL, longToFloat_decl_params, false, longToFloat_decl_result);
    REQUIRE(longToFloat_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *longToFloat = kefir_ir_module_new_function(mem, &module, longToFloat_decl->identifier, NULL, 1024);
    REQUIRE(longToFloat != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, longToFloat_decl->identifier));
    kefir_ir_type_append_v(longToFloat->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(longToFloat->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 3);
    kefir_irblock_append(&longToFloat->body, KEFIR_IROPCODE_INTCF32, 0);

    struct kefir_ir_type *longToDouble_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *longToDouble_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(longToDouble_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(longToDouble_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *longToDouble_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "longToDouble", NULL, longToDouble_decl_params, false, longToDouble_decl_result);
    REQUIRE(longToDouble_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *longToDouble = kefir_ir_module_new_function(mem, &module, longToDouble_decl->identifier, NULL, 1024);
    REQUIRE(longToDouble != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, longToDouble_decl->identifier));
    kefir_ir_type_append_v(longToDouble->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_ir_type_append_v(longToDouble->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 3);
    kefir_irblock_append(&longToDouble->body, KEFIR_IROPCODE_INTCF64, 0);

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}