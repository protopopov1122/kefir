#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/builtins.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    kefir_id_t locals_id, inttype_id, getint_id, unit_id;
    struct kefir_ir_type *unit_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL);
    struct kefir_ir_type *unit_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(unit_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(unit_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *unit_decl =
        kefir_ir_module_new_function_declaration(mem, &module,
            kefir_ir_module_symbol(mem, &module, "unit", &unit_id), NULL, unit_decl_params, false, unit_decl_result);
    REQUIRE(unit_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *unit = kefir_ir_module_new_function(mem, &module, unit_decl->identifier, NULL, 1024);
    REQUIRE(unit != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, unit_decl->identifier));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, unit_decl_params, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, unit_decl_result, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    
    struct kefir_ir_type *inttype = kefir_ir_module_new_type(mem, &module, 1, &inttype_id),
                       *getint_decl_params = kefir_ir_module_new_type(mem, &module, 2, NULL),
                       *getint_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *getint_locals = kefir_ir_module_new_type(mem, &module, 1, &locals_id);
    REQUIRE(inttype != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(getint_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *getint_decl =
        kefir_ir_module_new_function_declaration(mem, &module,
            kefir_ir_module_symbol(mem, &module, "getint", &getint_id), NULL, getint_decl_params, false, getint_decl_result);
    REQUIRE(getint_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *getint = kefir_ir_module_new_function(mem, &module, getint_decl->identifier, getint_locals, 1024);
    REQUIRE(getint != NULL, KEFIR_INTERNAL_ERROR);

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, inttype, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getint_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getint_decl_params, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getint_decl_result, KEFIR_IR_TYPE_INT, 0, 0));
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_PICK, 1);                    // 0: [C, V*, C]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_PUSH, 0);                    // 1: [C, V*, C, 0]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_IEQUALS, 0);                 // 2: [C, V*, C==0]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_BNOT, 0);                    // 3: [C, V*, C!=0]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_BRANCH, 7);                  // 4: [C, V*] ->@7
    kefir_irbuilder_block_append2(mem, &getint->body, KEFIR_IROPCODE_VARARG_GET, inttype_id, 0); // 5: [C, V]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_RET, 0);                     // 6: [C] return V
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_PICK, 0);                    // 7: [C, V*, V*]
    kefir_irbuilder_block_append2(mem, &getint->body, KEFIR_IROPCODE_VARARG_GET, inttype_id, 0); // 8: [C, V*, V]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_POP, 0);                     // 9: [C, V*]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_PICK, 1);                    // 10: [C, V*, C]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_IADD1, -1);                  // 11: [C, V*, C-1]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_XCHG, 2);                    // 12: [C-1, V*, C]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_POP, 0);                     // 13: [C-1, V*]
    kefir_irbuilder_block_append(mem, &getint->body, KEFIR_IROPCODE_JMP, 0);                     // 14: [C-1, V*] -> @0


    struct kefir_ir_type *getarg_decl_result = kefir_ir_module_new_type(mem, &module, 2, NULL),
                       *getarg_locals = kefir_ir_module_new_type(mem, &module, 1, &locals_id);
    REQUIRE(inttype != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(getarg_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *getarg_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "getarg", NULL, inttype, true, getarg_decl_result);
    REQUIRE(getarg_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *getarg = kefir_ir_module_new_function(mem, &module, getarg_decl->identifier, getarg_locals, 1024);
    REQUIRE(getarg != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, getarg_decl->identifier));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_decl_result, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_locals, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    kefir_irbuilder_block_append(mem, &getarg->body, KEFIR_IROPCODE_GETLOCALS, 0);               // 0: [C, L*]
    kefir_irbuilder_block_append2(mem, &getarg->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);   // 1: [C, V*]
    kefir_irbuilder_block_append(mem, &getarg->body, KEFIR_IROPCODE_PICK, 0);                    // 2: [C, V*, V*]
    kefir_irbuilder_block_append(mem, &getarg->body, KEFIR_IROPCODE_VARARG_START, 0);            // 3: [C, V*]
    kefir_irbuilder_block_append(mem, &getarg->body, KEFIR_IROPCODE_INVOKE, unit_id);            // 4: [C, V*]
    kefir_irbuilder_block_append(mem, &getarg->body, KEFIR_IROPCODE_INVOKE, getint_id);          // 5: [R]
    kefir_irbuilder_block_append(mem, &getarg->body, KEFIR_IROPCODE_GETLOCALS, 0);               // 6: [R, L*]
    kefir_irbuilder_block_append2(mem, &getarg->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);   // 7: [R, V*]
    kefir_irbuilder_block_append(mem, &getarg->body, KEFIR_IROPCODE_VARARG_END, 0);              // 8: [R]

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}