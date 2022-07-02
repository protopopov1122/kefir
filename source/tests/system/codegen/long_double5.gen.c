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
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout);

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    kefir_id_t func_params, func_returns, locals_id;
    struct kefir_ir_type *equals_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *equals_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *equals_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(equals_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(equals_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(equals_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *equals_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_equals", func_params, false, func_returns);
    REQUIRE(equals_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *equals_func = kefir_ir_module_new_function(mem, &module, equals_decl, locals_id, 1024);
    REQUIRE(equals_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, equals_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, equals_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, equals_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, equals_func->declaration->result, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &equals_func->body, KEFIR_IROPCODE_LDEQUALS, 0);

    struct kefir_ir_type *greater_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *greater_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *greater_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(greater_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(greater_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(greater_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *greater_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_greater", func_params, false, func_returns);
    REQUIRE(greater_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *greater_func = kefir_ir_module_new_function(mem, &module, greater_decl, locals_id, 1024);
    REQUIRE(greater_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, greater_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, greater_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, greater_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, greater_func->declaration->result, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &greater_func->body, KEFIR_IROPCODE_LDGREATER, 0);

    struct kefir_ir_type *lesser_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *lesser_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *lesser_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(lesser_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(lesser_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(lesser_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *lesser_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_lesser", func_params, false, func_returns);
    REQUIRE(lesser_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *lesser_func = kefir_ir_module_new_function(mem, &module, lesser_decl, locals_id, 1024);
    REQUIRE(lesser_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, lesser_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, lesser_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, lesser_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, lesser_func->declaration->result, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &lesser_func->body, KEFIR_IROPCODE_LDLESSER, 0);

    struct kefir_ir_type *trunc1_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *trunc1_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *trunc1_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(trunc1_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(trunc1_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(trunc1_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *trunc1_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_trunc1", func_params, false, func_returns);
    REQUIRE(trunc1_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *trunc1_func = kefir_ir_module_new_function(mem, &module, trunc1_decl, locals_id, 1024);
    REQUIRE(trunc1_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, trunc1_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, trunc1_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, trunc1_func->declaration->result, KEFIR_IR_TYPE_INT, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &trunc1_func->body, KEFIR_IROPCODE_LDTRUNC1, 0);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
