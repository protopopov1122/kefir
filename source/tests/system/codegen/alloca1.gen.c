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
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 2, NULL),
                         *decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "sum", decl_params, false, decl_result);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl, NULL, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name));
    kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, func->declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 128);  // [A1, A2, 128]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 0);    // [A1, A2, 128, 0]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_ALLOCA, 0);     // [A1, A2, PTR1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 2);       // [A1, A2, PTR1, A1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 1);       // [A1, A2, PTR1, A1, PTR1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);       // [A1, A2, PTR1, PTR1, A1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_STORE64, 0);    // [A1, A2, PTR1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 64);   // [A1, A2, PTR1, 64]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 16);   // [A1, A2, PTR1, 64, 16]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_ALLOCA, 0);     // [A1, A2, PTR1, PTR2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 2);       // [A1, A2, PTR1, PTR2, A2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 1);       // [A1, A2, PTR1, PTR2, A2, PTR2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);       // [A1, A2, PTR1, PTR2, PTR2, A2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_STORE64, 0);    // [A1, A2, PTR1, PTR2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);       // [A1, A2, PTR2, PTR1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_LOAD64, 0);     // [A1, A2, PTR2, A1]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_XCHG, 1);       // [A1, A2, A1, PTR2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_LOAD64, 0);     // [A1, A2, A1, A2]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IADD, 0);       // [A1, A2, RES]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 30);   // [A1, A2, RES, 30]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 4);    // [A1, A2, RES, 30, 4]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_ALLOCA, 0);     // [A1, A2, RES, PTR]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 0);       // [A1, A2, RES, PTR, PTR]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 2);       // [A1, A2, RES, PTR, PTR, RES]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_STORE64, 0);    // [A1, A2, RES, PTR]
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_LOAD64, 0);     // [A1, A2, RES, RES]

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
