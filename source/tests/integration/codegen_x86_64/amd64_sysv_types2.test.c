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
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    struct kefir_codegen_amd64_sysv_module sysv_module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_alloc(mem, &sysv_module, &module));
    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *decl_params = kefir_ir_module_new_type(mem, &module, 10, &func_params),
                         *decl_result = kefir_ir_module_new_type(mem, &module, 0, &func_returns);
    REQUIRE(decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1", func_params, false, func_returns);
    REQUIRE(decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func = kefir_ir_module_new_function(mem, &module, decl, KEFIR_ID_NONE, 0);
    REQUIRE(func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));
    codegen.asmgen.settings.enable_comments = false;
    codegen.asmgen.settings.enable_identation = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_STRUCT, 0, 7));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_SHORT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_UNION, 0, 3));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func->declaration->params, KEFIR_IR_TYPE_WORD, 0, 0));

    REQUIRE_OK(codegen.iface.translate(mem, &codegen.iface, &module));

    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_codegen_amd64_sysv_module_free(mem, &sysv_module));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}
