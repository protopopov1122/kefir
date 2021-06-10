#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout);

    kefir_id_t literal_id;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ir_type *decl1_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                         *decl1_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl1_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl1_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl1 =
        kefir_ir_module_new_function_declaration(mem, &module, "string1", decl1_params, false, decl1_result);
    REQUIRE(decl1 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 = kefir_ir_module_new_function(mem, &module, decl1, NULL, 1024);
    REQUIRE(func1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl1->name));
    kefir_irbuilder_type_append_v(mem, func1->declaration->result, KEFIR_IR_TYPE_WORD, 0, 0);

    const char *literal = "Hello, world!";
    REQUIRE_OK(kefir_ir_module_string_literal(mem, &module, literal, strlen(literal) + 1, &literal_id));
    kefir_irbuilder_block_appendi64(mem, &func1->body, KEFIR_IROPCODE_PUSHSTRING, literal_id);

    struct kefir_ir_type *decl2_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                         *decl2_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl2_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl2_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl2 =
        kefir_ir_module_new_function_declaration(mem, &module, "string2", decl2_params, false, decl2_result);
    REQUIRE(decl2 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func2 = kefir_ir_module_new_function(mem, &module, decl2, NULL, 1024);
    REQUIRE(func2 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl2->name));
    kefir_irbuilder_type_append_v(mem, func2->declaration->result, KEFIR_IR_TYPE_WORD, 0, 0);

    literal = "\n\n\t\tHey there\'\"!\v\n";
    REQUIRE_OK(kefir_ir_module_string_literal(mem, &module, literal, strlen(literal) + 1, &literal_id));
    kefir_irbuilder_block_appendi64(mem, &func2->body, KEFIR_IROPCODE_PUSHSTRING, literal_id);

    struct kefir_ir_type *decl3_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                         *decl3_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl3_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl3_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl3 =
        kefir_ir_module_new_function_declaration(mem, &module, "string3", decl3_params, false, decl3_result);
    REQUIRE(decl3 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func3 = kefir_ir_module_new_function(mem, &module, decl3, NULL, 1024);
    REQUIRE(func3 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl3->name));
    kefir_irbuilder_type_append_v(mem, func3->declaration->result, KEFIR_IR_TYPE_WORD, 0, 0);

    literal = "\0\0\0";
    REQUIRE_OK(kefir_ir_module_string_literal(mem, &module, literal, 4, &literal_id));
    kefir_irbuilder_block_appendi64(mem, &func3->body, KEFIR_IROPCODE_PUSHSTRING, literal_id);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
