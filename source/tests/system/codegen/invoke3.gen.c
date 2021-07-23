/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

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
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ir_type *proxysum_decl_params = kefir_ir_module_new_type(mem, &module, 8, NULL),
                         *proxysum_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(proxysum_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(proxysum_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *proxysum_decl = kefir_ir_module_new_function_declaration(
        mem, &module, "proxysum", proxysum_decl_params, false, proxysum_decl_result);
    REQUIRE(proxysum_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *proxysum = kefir_ir_module_new_function(mem, &module, proxysum_decl, NULL, 1024);
    REQUIRE(proxysum != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, proxysum_decl->name));

    struct kefir_ir_type *sumstruct_decl_params = kefir_ir_module_new_type(mem, &module, 9, NULL),
                         *sumstruct_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(sumstruct_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sumstruct_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sumstruct_decl = kefir_ir_module_new_function_declaration(
        mem, &module, "sumstruct", sumstruct_decl_params, false, sumstruct_decl_result);
    REQUIRE(sumstruct_decl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, sumstruct_decl->name));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_params, KEFIR_IR_TYPE_STRUCT, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_params, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_params, KEFIR_IR_TYPE_UNION, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_params, KEFIR_IR_TYPE_ARRAY, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_params, KEFIR_IR_TYPE_INT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_params, KEFIR_IR_TYPE_ARRAY, 0, 8));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_params, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, proxysum_decl_result, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    kefir_irbuilder_block_appendi64(mem, &proxysum->body, KEFIR_IROPCODE_PUSHI64, 5);
    kefir_irbuilder_block_appendi64(mem, &proxysum->body, KEFIR_IROPCODE_XCHG, 1);
    kefir_irbuilder_block_appendu64(mem, &proxysum->body, KEFIR_IROPCODE_INVOKE, sumstruct_decl->id);

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_INT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_STRUCT, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_UNION, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_ARRAY, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_INT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_ARRAY, 0, 8));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_params, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumstruct_decl_result, KEFIR_IR_TYPE_FLOAT64, 0, 0));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
