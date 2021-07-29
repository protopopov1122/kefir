/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ir_type *sumseq_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                         *sumseq_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(sumseq_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sumseq_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sumseq_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "sumseq", sumseq_decl_params, false, sumseq_decl_result);
    REQUIRE(sumseq_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *sumseq = kefir_ir_module_new_function(mem, &module, sumseq_decl, NULL, 1024);
    REQUIRE(sumseq != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, sumseq_decl->name));

    struct kefir_ir_type *fdbl_decl_params = kefir_ir_module_new_type(mem, &module, 1, NULL),
                         *fdbl_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(fdbl_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(fdbl_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *fdbl_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "fdbl", fdbl_decl_params, false, fdbl_decl_result);
    REQUIRE(fdbl_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *fdbl = kefir_ir_module_new_function(mem, &module, fdbl_decl, NULL, 1024);
    REQUIRE(fdbl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, fdbl_decl->name));

    struct kefir_ir_type *sum10_decl_params = kefir_ir_module_new_type(mem, &module, 10, NULL),
                         *sum10_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(sum10_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sum10_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sum10_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "sum10", sum10_decl_params, false, sum10_decl_result);
    REQUIRE(sum10_decl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, sum10_decl->name));

    struct kefir_ir_type *fsum_decl_params = kefir_ir_module_new_type(mem, &module, 3, NULL),
                         *fsum_decl_result = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE(fsum_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(fsum_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *fsum_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "fsum", fsum_decl_params, false, fsum_decl_result);
    REQUIRE(fsum_decl != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_external(mem, &module, fsum_decl->name));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumseq_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumseq_decl_result, KEFIR_IR_TYPE_INT, 0, 0));
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendi64(mem, &sumseq->body, KEFIR_IROPCODE_IADD1, 1);
    kefir_irbuilder_block_appendu64(mem, &sumseq->body, KEFIR_IROPCODE_INVOKE, sum10_decl->id);

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, fdbl_decl_params, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, fdbl_decl_result, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    kefir_irbuilder_block_appendi64(mem, &fdbl->body, KEFIR_IROPCODE_PICK, 0);
    kefir_irbuilder_block_appendu64(mem, &fdbl->body, KEFIR_IROPCODE_INVOKE, fsum_decl->id);

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sum10_decl_result, KEFIR_IR_TYPE_INT, 0, 0));

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, fsum_decl_params, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, fsum_decl_params, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, fsum_decl_result, KEFIR_IR_TYPE_FLOAT64, 0, 0));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
