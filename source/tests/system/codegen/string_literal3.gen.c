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
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    kefir_codegen_amd64_sysv_init(&codegen, stdout);

    kefir_id_t literal_id;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    kefir_id_t func_params, func_returns;
    struct kefir_ir_type *decl1_params = kefir_ir_module_new_type(mem, &module, 0, &func_params),
                         *decl1_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(decl1_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl1_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl1 =
        kefir_ir_module_new_function_declaration(mem, &module, "string1", func_params, false, func_returns);
    REQUIRE(decl1 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 = kefir_ir_module_new_function(mem, &module, decl1, NULL, 1024);
    REQUIRE(func1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl1->name));
    kefir_irbuilder_type_append_v(mem, func1->declaration->result, KEFIR_IR_TYPE_WORD, 0, 0);

    const kefir_char32_t literal[] = U"Hello, world!";
    REQUIRE_OK(kefir_ir_module_string_literal(mem, &module, KEFIR_IR_STRING_LITERAL_UNICODE32, true, literal,
                                              sizeof(literal) / sizeof(literal[0]), &literal_id));
    kefir_irbuilder_block_appendi64(mem, &func1->body, KEFIR_IROPCODE_PUSHSTRING, literal_id);

    struct kefir_ir_type *decl2_params = kefir_ir_module_new_type(mem, &module, 0, &func_params),
                         *decl2_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(decl2_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl2_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl2 =
        kefir_ir_module_new_function_declaration(mem, &module, "string2", func_params, false, func_returns);
    REQUIRE(decl2 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func2 = kefir_ir_module_new_function(mem, &module, decl2, NULL, 1024);
    REQUIRE(func2 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl2->name));
    kefir_irbuilder_type_append_v(mem, func2->declaration->result, KEFIR_IR_TYPE_WORD, 0, 0);

    const kefir_char32_t literal2[] = U"\n\n\t\tHey there\'\"!\v\n";
    REQUIRE_OK(kefir_ir_module_string_literal(mem, &module, KEFIR_IR_STRING_LITERAL_UNICODE32, true, literal2,
                                              sizeof(literal2) / sizeof(literal2[0]), &literal_id));
    kefir_irbuilder_block_appendi64(mem, &func2->body, KEFIR_IROPCODE_PUSHSTRING, literal_id);

    struct kefir_ir_type *decl3_params = kefir_ir_module_new_type(mem, &module, 0, &func_params),
                         *decl3_result = kefir_ir_module_new_type(mem, &module, 1, &func_returns);
    REQUIRE(decl3_params != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE(decl3_result != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function_decl *decl3 =
        kefir_ir_module_new_function_declaration(mem, &module, "string3", func_params, false, func_returns);
    REQUIRE(decl3 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func3 = kefir_ir_module_new_function(mem, &module, decl3, NULL, 1024);
    REQUIRE(func3 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, decl3->name));
    kefir_irbuilder_type_append_v(mem, func3->declaration->result, KEFIR_IR_TYPE_WORD, 0, 0);

    const kefir_char32_t literal3[] = U"\0\0\0";
    REQUIRE_OK(kefir_ir_module_string_literal(mem, &module, KEFIR_IR_STRING_LITERAL_UNICODE32, true, literal3,
                                              sizeof(literal3) / sizeof(literal3[0]), &literal_id));
    kefir_irbuilder_block_appendi64(mem, &func3->body, KEFIR_IROPCODE_PUSHSTRING, literal_id);

    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);

    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return EXIT_SUCCESS;
}
