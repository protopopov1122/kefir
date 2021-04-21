#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builtins.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    
    kefir_id_t locals_id, aggtype_id;
    struct kefir_ir_type *getarg_decl_params = kefir_ir_module_new_type(mem, &module, 2, NULL),
                       *getarg_decl_result = kefir_ir_module_new_type(mem, &module, 3, NULL),
                       *getarg_locals = kefir_ir_module_new_type(mem, &module, 1, &locals_id),
                       *aggtype = kefir_ir_module_new_type(mem, &module, 3, &aggtype_id);
    REQUIRE(getarg_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(getarg_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *getarg_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "getarg",
            getarg_decl_params, false, getarg_decl_result);
    REQUIRE(getarg_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *getarg = kefir_ir_module_new_function(mem, &module, getarg_decl, getarg_locals, 1024);
    REQUIRE(getarg != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, getarg_decl->name));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_decl_params, KEFIR_IR_TYPE_WORD, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, aggtype, KEFIR_IR_TYPE_STRUCT, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, aggtype, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, aggtype, KEFIR_IR_TYPE_LONG, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_decl_result, KEFIR_IR_TYPE_STRUCT, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_decl_result, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_decl_result, KEFIR_IR_TYPE_LONG, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_locals, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_GETLOCALS, 0);                    // 0: [C, S*, L*]
    kefir_irbuilder_block_appendu32(mem, &getarg->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);        // 1: [C, S*, V*]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_XCHG, 1);                         // 2: [C, V*, S*]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_VARARG_COPY, 0);                  // 3: [C]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_PICK, 0);                         // 4: [C, C]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_PUSHI64, 0);                         // 5: [C, C, 0]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_IEQUALS, 0);                      // 6: [C, C==0]
    kefir_irbuilder_block_appendu64(mem, &getarg->body, KEFIR_IROPCODE_BRANCH, 14);                      // 7: [C] -> @14
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_GETLOCALS, 0);                    // 8: [C, L*]
    kefir_irbuilder_block_appendu32(mem, &getarg->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);        // 9: [C, V*]
    kefir_irbuilder_block_appendu32(mem, &getarg->body, KEFIR_IROPCODE_VARARG_GET, aggtype_id, 0);      // 10: [C, A]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_POP, 0);                          // 11: [C]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_IADD1, -1);                       // 12: [C-1]
    kefir_irbuilder_block_appendu64(mem, &getarg->body, KEFIR_IROPCODE_JMP, 4);                          // 13: [C] -> @4
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_GETLOCALS, 0);                    // 14: [0, L*]
    kefir_irbuilder_block_appendu32(mem, &getarg->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);        // 15: [0, V*]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_PICK, 0);                         // 16: [0, V*, V*]
    kefir_irbuilder_block_appendu32(mem, &getarg->body, KEFIR_IROPCODE_VARARG_GET, aggtype_id, 0);      // 17: [0, V*, A]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_XCHG, 1);                         // 18: [0, A, V*]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_VARARG_END, 0);                   // 19: [0, A]

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}