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

#include "kefir/core/mem.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/test/util.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/format.h"
#include "./expression.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_util_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(
        kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, context, &env, &module, NULL));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
        mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("logical_and1", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_AND, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, false)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, false)));
    });

    FUNC("logical_and2", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_AND, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.067f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'C')));
    });

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                                          kefir_ast_type_specifier_void(mem)));

    FUNC("logical_and3", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_AND, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 4096)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                        mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))));
    });

    FUNC("logical_and4", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_AND, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 1.0l)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100)));
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_AND, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 2.0l)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 3.0l)));
    });

    FUNC("logical_or1", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_OR, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, false)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, false)));
    });

    FUNC("logical_or2", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_OR, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 0.4f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '7')));
    });

    struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name2->type_decl.specifiers,
                                                          kefir_ast_type_specifier_void(mem)));

    FUNC("logical_or3", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_OR, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 8192)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                        mem, type_name2, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))));
    });

    FUNC("logical_or4", {
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_OR, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, -1.0l)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 200)));
        BINARY_NODE(KEFIR_AST_OPERATION_LOGICAL_OR,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 2.0e10l)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, 3.0e-20l)));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
