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

    kefir_id_t locals_id, ldouble_type_id;
    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *ldouble_type = kefir_ir_module_new_type(mem, &module, 1, &ldouble_type_id),
                         *sumldouble_decl_params = kefir_ir_module_new_type(mem, &module, 1, &func_params),
                         *sumldouble_decl_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns),
                         *sumldouble_locals = kefir_ir_module_new_type(mem, &module, 1, &locals_id);
    REQUIRE(ldouble_type != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(sumldouble_decl_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *sumldouble_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "sumldouble", func_params, true, func_returns);
    REQUIRE(sumldouble_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *sumldouble =
        kefir_ir_module_new_function(mem, &module, sumldouble_decl, sumldouble_locals, 1024);
    REQUIRE(sumldouble != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, sumldouble_decl->name));

    kefir_codegen_amd64_sysv_init(&codegen, stdout);
    codegen.asmgen.settings.enable_comments = false;

    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, ldouble_type, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumldouble_decl_params, KEFIR_IR_TYPE_INT, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, sumldouble_decl_result, KEFIR_IR_TYPE_LONG_DOUBLE, 0, 0));
    REQUIRE_OK(
        kefir_irbuilder_type_append_v(mem, sumldouble_locals, KEFIR_IR_TYPE_BUILTIN, 0, KEFIR_IR_TYPE_BUILTIN_VARARG));
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_GETLOCALS, 0);             // 0: [C, L*]
    kefir_irbuilder_block_appendu32(mem, &sumldouble->body, KEFIR_IROPCODE_OFFSETPTR, locals_id, 0);  // 1: [C, V*]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_VARARG_START, 0);          // 2: [C]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_PUSHLD,
                                    kefir_ir_long_double_upper_half(1.0l));  // 3: [C, S1=0]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_PUSHU64,
                                    kefir_ir_long_double_lower_half(1.0l));                // 4: [C, S1=0, S2=0]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_PICK, 2);       // 5: [C, S1, S2, C]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_PUSHI64, 0);    // 6: [C, S1, S2, C, 0]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_IEQUALS, 0);    // 7: [C, S1, S2, C==0]
    kefir_irbuilder_block_appendu64(mem, &sumldouble->body, KEFIR_IROPCODE_BRANCH, 17);    // 8: [C, S1, S2] -> @16
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_GETLOCALS, 0);  // 9: [C, S1, S2, L*]
    kefir_irbuilder_block_appendu32(mem, &sumldouble->body, KEFIR_IROPCODE_OFFSETPTR, locals_id,
                                    0);  // 10: [C, S1, S2, V*]
    kefir_irbuilder_block_appendu32(mem, &sumldouble->body, KEFIR_IROPCODE_VARARG_GET, ldouble_type_id,
                                    0);                                                    // 11: [C, S1, S2, A1, A2]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_LDADD, 0);      // 12: [C, S1,S2 + A1,A2]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_XCHG, 2);       // 13: [S2, S1, C]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_IADD1, -1);     // 14: [S2, S1, C-1]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_XCHG, 2);       // 15: [C-1, S1, S2]
    kefir_irbuilder_block_appendu64(mem, &sumldouble->body, KEFIR_IROPCODE_JMP, 5);        // 16: [C-1, S1, S2] -> @4
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_GETLOCALS, 0);  // 17: [0, S1, S2, L*]
    kefir_irbuilder_block_appendu32(mem, &sumldouble->body, KEFIR_IROPCODE_OFFSETPTR, locals_id,
                                    0);                                                     // 18: [0, S1, S2, V*]
    kefir_irbuilder_block_appendi64(mem, &sumldouble->body, KEFIR_IROPCODE_VARARG_END, 0);  // 19: [0, S1, S2]

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
