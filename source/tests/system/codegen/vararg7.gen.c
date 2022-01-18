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
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/builtins.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    kefir_id_t locals_id, inttype_id;

    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *inttype = kefir_ir_module_new_type(mem, &module, 1, &inttype_id),
                         *getint_decl_params = kefir_ir_module_new_type(mem, &module, 2, &func_params),
                         *getint_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *getint_locals = kefir_ir_module_new_type(mem, &module, 1, &locals_id);
    REQUIRE(inttype != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(getint_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(getint_locals != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *getint_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "getint", func_params, false, func_returns);
    REQUIRE(getint_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *getint = kefir_ir_module_new_function(mem, &module, getint_decl, locals_id, 1024);
    REQUIRE(getint != NULL, KEFIR_INTERNAL_ERROR);

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, inttype, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getint_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(
        kefir_irbuilder_type_append_v(mem, getint_decl_params, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getint_decl_result, KEFIR_IR_TYPE_INT, 0, 0));
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_PICK, 1);                    // 0: [C, V*, C]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_PUSHI64, 0);                 // 1: [C, V*, C, 0]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_IEQUALS, 0);                 // 2: [C, V*, C==0]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_BNOT, 0);                    // 3: [C, V*, C!=0]
    kefir_irbuilder_block_appendu64(mem, &getint->body, KEFIR_IROPCODE_BRANCH, 7);                  // 4: [C, V*] ->@7
    kefir_irbuilder_block_appendu32(mem, &getint->body, KEFIR_IROPCODE_VARARG_GET, inttype_id, 0);  // 5: [C, V]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_RET, 0);                     // 6: [C] return V
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_PICK, 0);                    // 7: [C, V*, V*]
    kefir_irbuilder_block_appendu32(mem, &getint->body, KEFIR_IROPCODE_VARARG_GET, inttype_id, 0);  // 8: [C, V*, V]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_POP, 0);                     // 9: [C, V*]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_PICK, 1);                    // 10: [C, V*, C]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_IADD1, -1);                  // 11: [C, V*, C-1]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_XCHG, 2);                    // 12: [C-1, V*, C]
    kefir_irbuilder_block_appendi64(mem, &getint->body, KEFIR_IROPCODE_POP, 0);                     // 13: [C-1, V*]
    kefir_irbuilder_block_appendu64(mem, &getint->body, KEFIR_IROPCODE_JMP, 0);  // 14: [C-1, V*] -> @0

    struct kefir_ir_type *getarg_decl_result = kefir_ir_module_new_type(mem, &module, 2, &func_returns),
                         *getarg_locals = kefir_ir_module_new_type(mem, &module, 1, &locals_id);
    REQUIRE(inttype != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(getarg_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *getarg_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "getarg", inttype_id, true, func_returns);
    REQUIRE(getarg_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *getarg = kefir_ir_module_new_function(mem, &module, getarg_decl, locals_id, 1024);
    REQUIRE(getarg != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, getarg_decl->name));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, getarg_decl_result, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(
        kefir_irbuilder_type_append_v(mem, getarg_locals, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_GETLOCALS, 0);             // 0: [C, L*]
    kefir_irbuilder_block_appendu32(mem, &getarg->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);  // 1: [C, V*]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_PICK, 0);                  // 2: [C, V*, V*]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_VARARG_START, 0);          // 3: [C, V*]
    kefir_irbuilder_block_appendu64(mem, &getarg->body, KEFIR_IROPCODE_INVOKE, getint_decl->id);  // 5: [R]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_GETLOCALS, 0);             // 6: [R, L*]
    kefir_irbuilder_block_appendu32(mem, &getarg->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);  // 7: [R, V*]
    kefir_irbuilder_block_appendi64(mem, &getarg->body, KEFIR_IROPCODE_VARARG_END, 0);            // 8: [R]

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
