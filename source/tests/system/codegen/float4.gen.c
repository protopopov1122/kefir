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
    struct kefir_ir_type *floatToLong_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *floatToLong_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(floatToLong_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(floatToLong_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *floatToLong_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "floatToLong", func_params, false, func_returns);
    REQUIRE(floatToLong_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *floatToLong = kefir_ir_module_new_function(mem, &module, floatToLong_decl, NULL, 1024);
    REQUIRE(floatToLong != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, floatToLong_decl->name));
    kefir_irbuilder_type_append_v(mem, floatToLong->declaration->params, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, floatToLong->declaration->result, KEFIR_IR_TYPE_LONG, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &floatToLong->body, KEFIR_IROPCODE_F32CINT, 0);

    struct kefir_ir_type *doubleToLong_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *doubleToLong_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(doubleToLong_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(doubleToLong_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *doubleToLong_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "doubleToLong", func_params, false, func_returns);
    REQUIRE(doubleToLong_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *doubleToLong = kefir_ir_module_new_function(mem, &module, doubleToLong_decl, NULL, 1024);
    REQUIRE(doubleToLong != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, doubleToLong_decl->name));
    kefir_irbuilder_type_append_v(mem, doubleToLong->declaration->params, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, doubleToLong->declaration->result, KEFIR_IR_TYPE_LONG, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &doubleToLong->body, KEFIR_IROPCODE_F64CINT, 0);

    struct kefir_ir_type *longToFloat_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *longToFloat_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(longToFloat_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(longToFloat_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *longToFloat_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "longToFloat", func_params, false, func_returns);
    REQUIRE(longToFloat_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *longToFloat = kefir_ir_module_new_function(mem, &module, longToFloat_decl, NULL, 1024);
    REQUIRE(longToFloat != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, longToFloat_decl->name));
    kefir_irbuilder_type_append_v(mem, longToFloat->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, longToFloat->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &longToFloat->body, KEFIR_IROPCODE_INTCF32, 0);

    struct kefir_ir_type *longToDouble_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *longToDouble_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(longToDouble_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(longToDouble_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *longToDouble_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "longToDouble", func_params, false, func_returns);
    REQUIRE(longToDouble_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *longToDouble = kefir_ir_module_new_function(mem, &module, longToDouble_decl, NULL, 1024);
    REQUIRE(longToDouble != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, longToDouble_decl->name));
    kefir_irbuilder_type_append_v(mem, longToDouble->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, longToDouble->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &longToDouble->body, KEFIR_IROPCODE_INTCF64, 0);

    struct kefir_ir_type *floatToDouble_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *floatToDouble_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(floatToDouble_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(floatToDouble_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *floatToDouble_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "floatToDouble", func_params, false, func_returns);
    REQUIRE(floatToDouble_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *floatToDouble =
        kefir_ir_module_new_function(mem, &module, floatToDouble_decl, NULL, 1024);
    REQUIRE(floatToDouble != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, floatToDouble_decl->name));
    kefir_irbuilder_type_append_v(mem, floatToDouble->declaration->params, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, floatToDouble->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &floatToDouble->body, KEFIR_IROPCODE_F32CF64, 0);

    struct kefir_ir_type *doubleToFloat_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *doubleToFloat_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(doubleToFloat_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(doubleToFloat_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *doubleToFloat_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "doubleToFloat", func_params, false, func_returns);
    REQUIRE(doubleToFloat_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *doubleToFloat =
        kefir_ir_module_new_function(mem, &module, doubleToFloat_decl, NULL, 1024);
    REQUIRE(doubleToFloat != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, doubleToFloat_decl->name));
    kefir_irbuilder_type_append_v(mem, doubleToFloat->declaration->params, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, doubleToFloat->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &doubleToFloat->body, KEFIR_IROPCODE_F64CF32, 0);

    struct kefir_ir_type *ulongToFloat_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *ulongToFloat_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(ulongToFloat_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(ulongToFloat_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *ulongToFloat_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ulongToFloat", func_params, false, func_returns);
    REQUIRE(ulongToFloat_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *ulongToFloat = kefir_ir_module_new_function(mem, &module, ulongToFloat_decl, NULL, 1024);
    REQUIRE(ulongToFloat != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, ulongToFloat_decl->name));
    kefir_irbuilder_type_append_v(mem, ulongToFloat->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, ulongToFloat->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &ulongToFloat->body, KEFIR_IROPCODE_UINTCF32, 0);

    struct kefir_ir_type *ulongToDouble_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *ulongToDouble_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(ulongToDouble_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(ulongToDouble_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *ulongToDouble_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "ulongToDouble", func_params, false, func_returns);
    REQUIRE(ulongToDouble_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *ulongToDouble =
        kefir_ir_module_new_function(mem, &module, ulongToDouble_decl, NULL, 1024);
    REQUIRE(ulongToDouble != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, ulongToDouble_decl->name));
    kefir_irbuilder_type_append_v(mem, ulongToDouble->declaration->params, KEFIR_IR_TYPE_LONG, 0, 0);
    kefir_irbuilder_type_append_v(mem, ulongToDouble->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &ulongToDouble->body, KEFIR_IROPCODE_UINTCF64, 0);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
