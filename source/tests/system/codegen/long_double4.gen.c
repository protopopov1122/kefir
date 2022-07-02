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
    struct kefir_ir_type *sum_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *sum_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *sum_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(sum_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sum_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sum_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sum_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_sum", func_params, false, func_returns);
    REQUIRE(sum_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *sum_func = kefir_ir_module_new_function(mem, &module, sum_decl, locals_id, 1024);
    REQUIRE(sum_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, sum_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, sum_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, sum_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, sum_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &sum_func->body, KEFIR_IROPCODE_LDADD, 0);

    struct kefir_ir_type *sub_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *sub_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *sub_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(sub_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sub_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sub_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sub_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_sub", func_params, false, func_returns);
    REQUIRE(sum_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *sub_func = kefir_ir_module_new_function(mem, &module, sub_decl, locals_id, 1024);
    REQUIRE(sub_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, sub_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, sub_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, sub_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, sub_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &sub_func->body, KEFIR_IROPCODE_LDSUB, 0);

    struct kefir_ir_type *mul_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *mul_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *mul_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(mul_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(mul_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(mul_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *mul_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_mul", func_params, false, func_returns);
    REQUIRE(sum_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *mul_func = kefir_ir_module_new_function(mem, &module, mul_decl, locals_id, 1024);
    REQUIRE(mul_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, mul_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, mul_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, mul_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, mul_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &mul_func->body, KEFIR_IROPCODE_LDMUL, 0);

    struct kefir_ir_type *div_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *div_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *div_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(div_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(div_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(div_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *div_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_div", func_params, false, func_returns);
    REQUIRE(sum_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *div_func = kefir_ir_module_new_function(mem, &module, div_decl, locals_id, 1024);
    REQUIRE(div_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, div_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, div_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, div_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, div_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &div_func->body, KEFIR_IROPCODE_LDDIV, 0);

    struct kefir_ir_type *neg_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *neg_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *neg_locals = kefir_ir_module_new_type(mem, &module, 0, &locals_id);
    REQUIRE(neg_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(neg_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(neg_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *neg_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_neg", func_params, false, func_returns);
    REQUIRE(sum_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *neg_func = kefir_ir_module_new_function(mem, &module, neg_decl, locals_id, 1024);
    REQUIRE(neg_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, neg_decl->name, KEFIR_IR_IDENTIFIER_GLOBAL));
    kefir_irbuilder_type_append_v(mem, neg_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, neg_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &neg_func->body, KEFIR_IROPCODE_LDNEG, 0);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
