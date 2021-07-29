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

#include "kefir/core/mem.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/test/util.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/format.h"
#include "./expression.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context, "variable", kefir_ast_type_signed_int(),
                                                         NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));

    struct kefir_ast_translator_context global_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&global_translator_context, &global_context.context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC2("for1", {
        struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
            mem, kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "body")),
            NULL);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                              kefir_ast_storage_class_specifier_extern(mem)));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_void(mem)));

        struct kefir_ast_expression_statement *body = kefir_ast_new_expression_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_function_call(
                     mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "body")))));

        struct kefir_ast_for_statement *for1 =
            kefir_ast_new_for_statement(mem, NULL, NULL, NULL, KEFIR_AST_NODE_BASE(body));

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(decl1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(for1)));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC2("for2", {
        struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
            mem, kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "body")),
            NULL);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                              kefir_ast_storage_class_specifier_extern(mem)));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_void(mem)));

        struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
            mem, kefir_ast_declarator_identifier(mem, context->symbols, "i"),
            kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_int(mem)));

        struct kefir_ast_node_base *ctrl_expr = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_LESS, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10))));

        struct kefir_ast_node_base *tail_expr = KEFIR_AST_NODE_BASE(
            kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i"))));

        struct kefir_ast_expression_statement *body = kefir_ast_new_expression_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_function_call(
                     mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "body")))));

        struct kefir_ast_for_statement *for1 = kefir_ast_new_for_statement(mem, KEFIR_AST_NODE_BASE(decl2), ctrl_expr,
                                                                           tail_expr, KEFIR_AST_NODE_BASE(body));

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(decl1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(for1)));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC2("for3", {
        struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
            mem, kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "body2")),
            NULL);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                              kefir_ast_storage_class_specifier_extern(mem)));
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_bool(mem)));

        struct kefir_ast_declaration *decl2 =
            kefir_ast_new_declaration(mem, kefir_ast_declarator_identifier(mem, context->symbols, "i"), NULL);
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_int(mem)));

        struct kefir_ast_node_base *init_expr = KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))));

        struct kefir_ast_node_base *ctrl_expr = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_LESS, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10))));

        struct kefir_ast_node_base *tail_expr = KEFIR_AST_NODE_BASE(
            kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                                          KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "i"))));

        struct kefir_ast_node_base *body = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
            mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_function_call(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "body2")))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_continue_statement(mem)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem))));

        struct kefir_ast_for_statement *for1 = kefir_ast_new_for_statement(mem, init_expr, ctrl_expr, tail_expr, body);

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(decl1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(decl2)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(for1)));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
