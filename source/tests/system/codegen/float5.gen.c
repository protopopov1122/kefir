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
    struct kefir_ir_type *fneg_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *fneg_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(fneg_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(fneg_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *fneg_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "fneg", func_params, false, func_returns);
    REQUIRE(fneg_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *fneg = kefir_ir_module_new_function(mem, &module, fneg_decl, KEFIR_ID_NONE, 1024);
    REQUIRE(fneg != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, fneg_decl->name));
    kefir_irbuilder_type_append_v(mem, fneg->declaration->params, KEFIR_IR_TYPE_FLOAT32, 0, 0);
    kefir_irbuilder_type_append_v(mem, fneg->declaration->result, KEFIR_IR_TYPE_FLOAT32, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &fneg->body, KEFIR_IROPCODE_F32NEG, 0);

    struct kefir_ir_type *dneg_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *dneg_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(dneg_decl_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(dneg_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *dneg_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "dneg", func_params, false, func_returns);
    REQUIRE(dneg_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *dneg = kefir_ir_module_new_function(mem, &module, dneg_decl, KEFIR_ID_NONE, 1024);
    REQUIRE(dneg != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, dneg_decl->name));
    kefir_irbuilder_type_append_v(mem, dneg->declaration->params, KEFIR_IR_TYPE_FLOAT64, 0, 0);
    kefir_irbuilder_type_append_v(mem, dneg->declaration->result, KEFIR_IR_TYPE_FLOAT64, 0, 3);
    kefir_irbuilder_block_appendi64(mem, &dneg->body, KEFIR_IROPCODE_F64NEG, 0);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
