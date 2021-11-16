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
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context, "extern_variable1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context, "extern_variable2", kefir_ast_type_char(),
                                                        NULL, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_static(mem, &local_context, "static_variable1", kefir_ast_type_float(),
                                                     NULL, NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context, "local_var1", kefir_ast_type_boolean(), NULL,
                                                   NULL, NULL, NULL));

    struct kefir_ast_constant_expression *const_expr_X =
        kefir_ast_new_constant_expression(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10)));
    REQUIRE_OK(kefir_ast_analyze_constant_expression(mem, context, const_expr_X));
    REQUIRE_OK(kefir_ast_constant_expression_evaluate(mem, context, const_expr_X));
    REQUIRE_OK(kefir_ast_local_context_define_constant(mem, &local_context, "X", const_expr_X,
                                                       context->type_traits->underlying_enumeration_type, NULL, NULL));

    struct kefir_ast_node_base *node1 =
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "extern_variable1"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node1));
    struct kefir_ast_node_base *node2 =
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "extern_variable2"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node2));
    struct kefir_ast_node_base *node3 =
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "static_variable1"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node3));
    struct kefir_ast_node_base *node4 =
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "local_var1"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node4));
    struct kefir_ast_node_base *node5 = KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node5));

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

    struct kefir_ir_type *func1_params = kefir_ir_module_new_type(mem, &module, 0, NULL);
    struct kefir_ir_type *func1_returns = kefir_ir_module_new_type(mem, &module, 0, NULL);

    struct kefir_ir_function_decl *func1_decl =
        kefir_ir_module_new_function_declaration(mem, &module, "func1", func1_params, false, func1_returns);
    REQUIRE(func1_decl != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ir_function *func1 =
        kefir_ir_module_new_function(mem, &module, func1_decl, translator_local_scope.local_layout, 0);
    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func1->body));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node1, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node2, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node3, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node4, &builder, &translator_context));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node5, &builder, &translator_context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node1));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node2));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node3));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node4));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node5));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
