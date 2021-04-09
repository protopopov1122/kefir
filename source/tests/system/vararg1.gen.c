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
    const char *FMT = "INTEGER: %i\n";

    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    
    struct kefir_ir_type *printint_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *printint_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(printint_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(printint_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *printint_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "printint", NULL, printint_decl_params, false, printint_decl_result);
    REQUIRE(printint_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *printint = kefir_ir_module_new_function(mem, &module, printint_decl->identifier, NULL, 1024);
    REQUIRE(printint != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, printint_decl->identifier));

    kefir_id_t sprintf_id;
    struct kefir_ir_type *sprintf_decl_params = kefir_ir_module_new_type(mem, &module, 3, NULL),
                       *sprintf_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(sprintf_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sprintf_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sprintf_decl =
        kefir_ir_module_new_function_declaration(mem, &module,
            kefir_ir_module_symbol(mem, &module, "sprintf_vararg2", &sprintf_id),
            "sprintf", sprintf_decl_params, true, sprintf_decl_result);
    REQUIRE(sprintf_decl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, "sprintf"));

    kefir_id_t fmt_id;
    struct kefir_ir_type *string_type = kefir_ir_module_new_type(mem, &module, 2, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, string_type, KEFIR_IR_TYPE_ARRAY, 0, strlen(FMT) + 1));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, string_type, KEFIR_IR_TYPE_CHAR, 0, 0));
    struct kefir_ir_data *fmt_data1 = kefir_ir_module_new_named_data(mem, &module, kefir_ir_module_symbol(mem, &module, "fmt", &fmt_id), string_type);
    REQUIRE_OK(kefir_ir_data_set_string(fmt_data1, 0, FMT));
    REQUIRE_OK(kefir_ir_data_finalize(fmt_data1));

    kefir_id_t result_id;
    struct kefir_ir_type *string2_type = kefir_ir_module_new_type(mem, &module, 2, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, string2_type, KEFIR_IR_TYPE_ARRAY, 0, 256));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, string2_type, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_ir_data_finalize(kefir_ir_module_new_named_data(mem, &module,
        kefir_ir_module_symbol(mem, &module, "result", &result_id), string2_type)));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, printint_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, printint_decl_result, KEFIR_IR_TYPE_WORD, 0, 0));
    kefir_irbuilder_block_appendu64(mem, &printint->body, KEFIR_IROPCODE_GETGLOBAL, result_id);     // 0: [I, R*]
    kefir_irbuilder_block_appendi64(mem, &printint->body, KEFIR_IROPCODE_PICK, 0);                  // 1: [I, R*, R*]
    kefir_irbuilder_block_appendu64(mem, &printint->body, KEFIR_IROPCODE_GETGLOBAL, fmt_id);        // 2: [I, R*, R*, F*]
    kefir_irbuilder_block_appendi64(mem, &printint->body, KEFIR_IROPCODE_PICK, 3);                  // 3: [I, R*, R*, F*, I]
    kefir_irbuilder_block_appendu64(mem, &printint->body, KEFIR_IROPCODE_INVOKE, sprintf_id);       // 4: [I, R*, O]
    kefir_irbuilder_block_appendi64(mem, &printint->body, KEFIR_IROPCODE_POP, 0);                   // 5: [I, R*]

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sprintf_decl_params, KEFIR_IR_TYPE_WORD, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sprintf_decl_params, KEFIR_IR_TYPE_WORD, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sprintf_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sprintf_decl_result, KEFIR_IR_TYPE_INT, 0, 0));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}