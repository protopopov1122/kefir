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

    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *to_long_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *to_long_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *to_long_locals = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(to_long_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(to_long_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *to_long_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_to_long", func_params, false, func_returns);
    REQUIRE(to_long_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *to_long_func =
        kefir_ir_module_new_function(mem, &module, to_long_decl, to_long_locals, 1024);
    REQUIRE(to_long_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, to_long_decl->name));
    kefir_irbuilder_type_append_v(mem, to_long_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, to_long_func->declaration->result, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &to_long_func->body, KEFIR_IROPCODE_LDCINT, 0);

    struct kefir_ir_type *from_long_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *from_long_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *from_long_locals = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(from_long_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(from_long_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *from_long_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_from_long", func_params, false, func_returns);
    REQUIRE(from_long_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *from_long_func =
        kefir_ir_module_new_function(mem, &module, from_long_decl, from_long_locals, 1024);
    REQUIRE(from_long_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, from_long_decl->name));
    kefir_irbuilder_type_append_v(mem, from_long_func->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, from_long_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &from_long_func->body, KEFIR_IROPCODE_INTCLD, 0);

    struct kefir_ir_type *from_ulong_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *from_ulong_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *from_ulong_locals = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(from_ulong_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(from_ulong_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *from_ulong_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_from_ulong", func_params, false, func_returns);
    REQUIRE(from_ulong_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *from_ulong_func =
        kefir_ir_module_new_function(mem, &module, from_ulong_decl, from_ulong_locals, 1024);
    REQUIRE(from_ulong_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, from_ulong_decl->name));
    kefir_irbuilder_type_append_v(mem, from_ulong_func->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, from_ulong_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &from_ulong_func->body, KEFIR_IROPCODE_UINTCLD, 0);

    struct kefir_ir_type *from_float_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *from_float_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *from_float_locals = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(from_float_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(from_float_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *from_float_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_from_float", func_params, false, func_returns);
    REQUIRE(from_float_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *from_float_func =
        kefir_ir_module_new_function(mem, &module, from_float_decl, from_float_locals, 1024);
    REQUIRE(from_float_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, from_float_decl->name));
    kefir_irbuilder_type_append_v(mem, from_float_func->declaration->params, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, from_float_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &from_float_func->body, KEFIR_IROPCODE_F32CLD, 0);

    struct kefir_ir_type *from_double_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *from_double_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *from_double_locals = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(from_double_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(from_double_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *from_double_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_from_double", func_params, false, func_returns);
    REQUIRE(from_double_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *from_double_func =
        kefir_ir_module_new_function(mem, &module, from_double_decl, from_double_locals, 1024);
    REQUIRE(from_double_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, from_double_decl->name));
    kefir_irbuilder_type_append_v(mem, from_double_func->declaration->params, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, from_double_func->declaration->result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &from_double_func->body, KEFIR_IROPCODE_F64CLD, 0);

    struct kefir_ir_type *to_float_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *to_float_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *to_float_locals = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(to_float_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(to_float_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *to_float_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_to_float", func_params, false, func_returns);
    REQUIRE(to_float_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *to_float_func =
        kefir_ir_module_new_function(mem, &module, to_float_decl, to_float_locals, 1024);
    REQUIRE(to_float_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, to_float_decl->name));
    kefir_irbuilder_type_append_v(mem, to_float_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, to_float_func->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &to_float_func->body, KEFIR_IROPCODE_LDCF32, 0);

    struct kefir_ir_type *to_double_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *to_double_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *to_double_locals = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(to_double_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(to_double_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *to_double_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ldouble_to_double", func_params, false, func_returns);
    REQUIRE(to_double_decl != NULL, KEFIR_INTERNAL_ERROR);

    struct kefir_ir_function *to_double_func =
        kefir_ir_module_new_function(mem, &module, to_double_decl, to_double_locals, 1024);
    REQUIRE(to_double_func != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, to_double_decl->name));
    kefir_irbuilder_type_append_v(mem, to_double_func->declaration->params, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0);
    kefir_irbuilder_type_append_v(mem, to_double_func->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_block_appendi64(mem, &to_double_func->body, KEFIR_IROPCODE_LDCF64, 0);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
