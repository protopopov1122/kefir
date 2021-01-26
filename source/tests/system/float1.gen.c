#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout, mem);

    kefir_ir_module_id_t locals_id;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                       *decl_result = kefir_ir_module_new_type(mem, &module, 4, NULL),
                       *func_locals = kefir_ir_module_new_type(mem, &module, 4, &locals_id);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "circle", NULL, decl_params, false, decl_result);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl->identifier, func_locals, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->identifier));
    kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_STRUCT, 0, 3);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_STRUCT, 0, 3);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_FLOAT32, 0, 0);

    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_GETLOCALS, 0);              // [V, L*]
    kefir_irbuilder_block_append2(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);  // [V, R*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_PICK, 0);                   // [V, R*, R*]
    kefir_irbuilder_block_append2(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 1);  // [V, R*, R1*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_PICK, 2);                   // [V, R*, R1*, V]
    kefir_irbuilder_block_appendf2(mem, &func->body, KEFIR_IROPCODE_PUSHF32, 3.14159f, 0.0f); // [V, R*, R1*, V, PI]
    kefir_irbuilder_block_appendf2(mem, &func->body, KEFIR_IROPCODE_PUSHF32, 2.0f, 0.0f);     // [V, R*, R1*, V, PI, 2f]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_F32MUL, 0);                 // [V, R*, R1*, V, 2*PI]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_F32MUL, 0);                 // [V, R*, R1*, 2*PI*V]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);                   // [V, R*, 2*PI*V, R1*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_STORE32, 0);                // [V, R*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_PICK, 0);                   // [V, R*, R*]
    kefir_irbuilder_block_append2(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 2);  // [V, R*, R2*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_PICK, 2);                   // [V, R*, R2*, V]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_PICK, 0);                   // [V, R*, R2*, V, V]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_F32MUL, 0);                 // [V, R*, R2*, V*V]
    kefir_irbuilder_block_appendf2(mem, &func->body, KEFIR_IROPCODE_PUSHF32, 3.14159f, 0.0f); // [V, R*, R2*, V*V, PI]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_F32MUL, 0);                 // [V, R*, R2*, V*V*PI]
    kefir_irbuilder_block_appendf2(mem, &func->body, KEFIR_IROPCODE_PUSHF32, 2.0f, 0.0f);     // [V, R*, R2*, V*V*PI, 2f]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_F32DIV, 0);                 // [V, R*, R2*, V*V*PI/2]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);                   // [V, R*, V*V*PI/2, R2*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_STORE32, 0);                // [V, R*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_PICK, 0);                   // [V, R*, R*]
    kefir_irbuilder_block_append2(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 3);  // [V, R*, R2*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_PICK, 2);                   // [V, R*, R2*, V]
    kefir_irbuilder_block_appendf2(mem, &func->body, KEFIR_IROPCODE_PUSHF32, 0.0f, 0.0f);     // [V, R*, R2*, V, 0f]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);                   // [V, R*, R2*, 0f, V]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_F32SUB, 0);                 // [V, R*, R2*, -V]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);                   // [V, R*, -V, R2*]
    kefir_irbuilder_block_append(mem, &func->body, KEFIR_IROPCODE_STORE32, 0);                // [V, R*]

    KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}