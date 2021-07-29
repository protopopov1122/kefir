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
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));
    codegen.asmgen.settings.enable_comments = false;
    codegen.asmgen.settings.enable_identation = false;

    struct kefir_ir_type *extractu_decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                         *extractu_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(extractu_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(extractu_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *extractu_decl = kefir_ir_module_new_function_declaration(
        mem, &module, "extractu", extractu_decl_params, false, extractu_decl_result);
    REQUIRE(extractu_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *extractu_func = kefir_ir_module_new_function(mem, &module, extractu_decl, NULL, 1024);
    REQUIRE(extractu_func != NULL, KEFIR_INTERNAL_ERROR);

    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 0, 10));

    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 1, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 2, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 4, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 8, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 16, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 32, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 1, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 2, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 4, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 8, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extractu_func->body, KEFIR_IROPCODE_EXTUBITS, 16, 10));

    struct kefir_ir_type *extracts_decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                         *extracts_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(extracts_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(extracts_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *extracts_decl = kefir_ir_module_new_function_declaration(
        mem, &module, "extracts", extracts_decl_params, false, extracts_decl_result);
    REQUIRE(extracts_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *extracts_func = kefir_ir_module_new_function(mem, &module, extracts_decl, NULL, 1024);
    REQUIRE(extracts_func != NULL, KEFIR_INTERNAL_ERROR);

    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 0, 10));

    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 1, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 2, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 4, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 8, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 16, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 32, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 1, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 2, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 4, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 8, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &extracts_func->body, KEFIR_IROPCODE_EXTSBITS, 16, 10));

    struct kefir_ir_type *insert_decl_params = kefir_ir_module_new_type(mem, &module, 0, NULL),
                         *insert_decl_result = kefir_ir_module_new_type(mem, &module, 0, NULL);
    REQUIRE(insert_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(insert_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *insert_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "insert", insert_decl_params, false, insert_decl_result);
    REQUIRE(insert_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *insert_func = kefir_ir_module_new_function(mem, &module, insert_decl, NULL, 1024);
    REQUIRE(insert_func != NULL, KEFIR_INTERNAL_ERROR);

    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 0, 10));

    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 1, 0));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 2, 1));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 4, 2));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 8, 3));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 16, 4));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 32, 5));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 1, 6));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 2, 7));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 4, 8));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 8, 9));
    REQUIRE_OK(kefir_irbuilder_block_appendu32(mem, &insert_func->body, KEFIR_IROPCODE_INSERTBITS, 16, 10));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}
