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
    struct kefir_ir_type *func1_decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                         *func1_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(func1_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(func1_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *func1_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1", func1_decl_params, false, func1_decl_result);
    REQUIRE(func1_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 = kefir_ir_module_new_function(mem, &module, func1_decl, NULL, 1024);
    REQUIRE(func1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, func1_decl->name));

    struct kefir_ir_type *func2_decl_params = kefir_ir_module_new_type(mem, &module, 3, NULL),
                         *func2_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(func2_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(func2_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *func2_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func2", func2_decl_params, false, func2_decl_result);
    REQUIRE(func2_decl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, func2_decl->name));

    struct kefir_ir_type *func3_decl_params = kefir_ir_module_new_type(mem, &module, 3, NULL),
                         *func3_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(func3_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(func3_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *func3_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func3", func3_decl_params, false, func3_decl_result);
    REQUIRE(func3_decl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, func3_decl->name));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    kefir_irbuilder_block_appendu64(mem, &func1->body, KEFIR_IROPCODE_INVOKE, func2_decl->id);
    kefir_irbuilder_block_appendu64(mem, &func1->body, KEFIR_IROPCODE_INVOKE, func3_decl->id);

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func2_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func2_decl_params, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func2_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func2_decl_result, KEFIR_IR_TYPE_INT, 0, 0));

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func3_decl_params, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func3_decl_params, KEFIR_IR_TYPE_BOOL, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func3_decl_params, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, func3_decl_result, KEFIR_IR_TYPE_FLOAT64, 0, 0));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}
