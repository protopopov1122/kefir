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

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "factorial", func_params, false, func_returns);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl, KEFIR_ID_NONE, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);      // 0:  [C; C]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 1);   // 1:  [C; C; 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IGREATER, 0);  // 2:  [C; C > 1]
    kefir_irbuilder_block_appendu64(mem, &func->body, KEFIR_IROPCODE_BRANCH, 6);    // 3:  [C]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 1);   // 4:  [C; 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_RET, 0);       // 5:  -> 1
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);      // 6:  [C; S]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 1);      // 7:  [C; S; C]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IADD1, -1);    // 8:  [C; S; C - 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);      // 9:  [C; S; C - 1; C - 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 1);   // 10: [C; S; C - 1; C - 1; 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IGREATER, 0);  // 11: [C; S; C - 1; C - 1 > 1]
    kefir_irbuilder_block_appendu64(mem, &func->body, KEFIR_IROPCODE_BRANCH, 15);   // 12: [C; S; C - 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_POP, 0);       // 13: [C; S]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_RET, 0);       // 14: -> S
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);      // 15: [C; S; C - 1; C - 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUT, 3);       // 16: [C - 1; S; C - 1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IMUL, 0);      // 17: [C - 1; S * (C - 1)]
    kefir_irbuilder_block_appendu64(mem, &func->body, KEFIR_IROPCODE_JMP, 7);       // 18: [C - 1; S * (C - 1)]

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
