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
#include "kefir/ir/module.h"
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 0, &func_params),
                         *decl_result = kefir_ir_module_new_type(mem, &module, 0, &func_returns);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1", func_params, false, func_returns);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl, NULL, 1024);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name));
    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_NOP, 0);
    kefir_irbuilder_block_appendu64(mem, &func->body, KEFIR_IROPCODE_JMP, 1);
    kefir_irbuilder_block_appendu64(mem, &func->body, KEFIR_IROPCODE_BRANCH, 2);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_RET, 0);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PUSHI64, 1000);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_POP, 0);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_PICK, 1);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_DROP, 2);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IADD, 0);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_ISUB, 2);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IMUL, 3);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IDIV, 4);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IMOD, 5);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_INEG, 6);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_INOT, 7);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IAND, 8);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IOR, 9);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IXOR, 10);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IRSHIFT, 11);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IARSHIFT, 12);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_ILSHIFT, 13);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IEQUALS, 14);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_IGREATER, 15);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_ILESSER, 16);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_BAND, 17);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_BOR, 18);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_BNOT, 19);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_TRUNCATE1, 20);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_EXTEND8, 21);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_EXTEND16, 22);
    kefir_irbuilder_block_appendi64(mem, &func->body, KEFIR_IROPCODE_EXTEND32, 23);
    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));

    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}
