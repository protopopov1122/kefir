/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout);

    kefir_id_t locals_id;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *decl_result = kefir_ir_module_new_type(mem, &module, 4, &func_returns),
                         *func_locals = kefir_ir_module_new_type(mem, &module, 4, &locals_id);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "circle", func_params, false, func_returns);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl, locals_id, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_STRUCT, 0, 3);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_STRUCT, 0, 3);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, func_locals, KEFIR_IR_TYPE_FLOAT64, 0, 0);

    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_GETLOCALS, 0);             // [V, L*]
    kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);  // [V, R*]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);                  // [V, R*, R*]
    kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 1);  // [V, R*, R1*]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 2);                  // [V, R*, R1*, V]
    kefir_irbuilder_block_appendf64(mem, &func->body, KEFIR_IROPCODE_PUSHF64, 3.14159);         // [V, R*, R1*, V, PI]
    kefir_irbuilder_block_appendf64(mem, &func->body, KEFIR_IROPCODE_PUSHF64, 2.0);  // [V, R*, R1*, V, PI, 2f]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_F64MUL, 0);     // [V, R*, R1*, V, 2*PI]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_F64MUL, 0);     // [V, R*, R1*, 2*PI*V]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_STORE64, 0);    // [V, R*]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);       // [V, R*, R*]
    kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 2);  // [V, R*, R2*]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 2);                  // [V, R*, R2*, V]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);                  // [V, R*, R2*, V, V]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_F64MUL, 0);                // [V, R*, R2*, V*V]
    kefir_irbuilder_block_appendf64(mem, &func->body, KEFIR_IROPCODE_PUSHF64, 3.14159);         // [V, R*, R2*, V*V, PI]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_F64MUL, 0);                // [V, R*, R2*, V*V*PI]
    kefir_irbuilder_block_appendf64(mem, &func->body, KEFIR_IROPCODE_PUSHF64, 2.0);  // [V, R*, R2*, V*V*PI, 2f]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_F64DIV, 0);     // [V, R*, R2*, V*V*PI/2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_STORE64, 0);    // [V, R*]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);       // [V, R*, R*]
    kefir_irbuilder_block_appendu32(mem, &func->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 3);  // [V, R*, R2*]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 2);                  // [V, R*, R2*, V]
    kefir_irbuilder_block_appendf64(mem, &func->body, KEFIR_IROPCODE_PUSHF64, 0.0);             // [V, R*, R2*, V, 0f]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);                  // [V, R*, R2*, 0f, V]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_F64SUB, 0);                // [V, R*, R2*, -V]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_STORE64, 0);               // [V, R*]

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
