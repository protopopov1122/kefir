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

#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/function_declaration_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_node_analysis_while_statements1, "AST node analysis - while statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_while_statement *stmt1 =
        kefir_ast_new_while_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->type ==
           KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt1->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt1->controlling_expr->properties.type, kefir_ast_type_bool()));
    ASSERT(stmt1->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_while_statement *stmt2 = kefir_ast_new_while_statement(
        &kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))),
        KEFIR_AST_NODE_BASE(compound1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt2->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->type ==
           KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt2->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt2->controlling_expr->properties.type,
                               kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_double())));
    ASSERT(stmt2->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_while_statements2, "AST node analysis - while statements #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL));
    ASSERT(node1 != NULL);

    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'C'));
    ASSERT(node2 != NULL);

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_while_statement *stmt1 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node1), KEFIR_AST_NODE_CLONE(&kft_mem, node1));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));

    struct kefir_ast_while_statement *stmt2 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node2), KEFIR_AST_NODE_CLONE(&kft_mem, node2));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));

    struct kefir_ast_while_statement *stmt3 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node2), KEFIR_AST_NODE_CLONE(&kft_mem, node1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));

    struct kefir_ast_while_statement *stmt4 =
        kefir_ast_new_while_statement(&kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)),
                                      KEFIR_AST_NODE_CLONE(&kft_mem, node1));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt4)));

    struct kefir_ast_while_statement *stmt5 =
        kefir_ast_new_while_statement(&kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node2),
                                      KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt5)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node2));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt4)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt5)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_do_while_statements1, "AST node analysis - do while statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_do_while_statement *stmt1 =
        kefir_ast_new_do_while_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                         KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_DO);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt1->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt1->controlling_expr->properties.type, kefir_ast_type_bool()));
    ASSERT(stmt1->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_do_while_statement *stmt2 = kefir_ast_new_do_while_statement(
        &kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))),
        KEFIR_AST_NODE_BASE(compound1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt2->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_DO);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt2->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt2->controlling_expr->properties.type,
                               kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_double())));
    ASSERT(stmt2->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(context->resolve_ordinary_identifier(context, "x", &scoped_id));
    ASSERT(context->resolve_ordinary_identifier(context, "y", &scoped_id) == KEFIR_NOT_FOUND);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_do_while_statements2, "AST node analysis - do while statements #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL));
    ASSERT(node1 != NULL);

    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'C'));
    ASSERT(node2 != NULL);

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_do_while_statement *stmt1 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node1), KEFIR_AST_NODE_CLONE(&kft_mem, node1));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));

    struct kefir_ast_do_while_statement *stmt2 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node2), KEFIR_AST_NODE_CLONE(&kft_mem, node2));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));

    struct kefir_ast_do_while_statement *stmt3 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node2), KEFIR_AST_NODE_CLONE(&kft_mem, node1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));

    struct kefir_ast_do_while_statement *stmt4 =
        kefir_ast_new_do_while_statement(&kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)),
                                         KEFIR_AST_NODE_CLONE(&kft_mem, node1));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt4)));

    struct kefir_ast_do_while_statement *stmt5 =
        kefir_ast_new_do_while_statement(&kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node2),
                                         KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt5)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node2));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt4)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt5)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_for_statements1, "AST node analysis - for statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_for_statement *stmt1 = kefir_ast_new_for_statement(
        &kft_mem, NULL, NULL, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt1->init == NULL);
    ASSERT(stmt1->controlling_expr == NULL);
    ASSERT(stmt1->tail == NULL);
    ASSERT(stmt1->body != NULL);
    ASSERT(stmt1->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_for_statement *stmt2 = kefir_ast_new_for_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, -1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(&kft_mem, 0)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(&kft_mem, 1)), KEFIR_AST_NODE_BASE(compound1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt2->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt2->init != NULL);
    ASSERT(stmt2->init->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt2->init->properties.type, kefir_ast_type_signed_long()));
    ASSERT(stmt2->controlling_expr != NULL);
    ASSERT(stmt2->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt2->controlling_expr->properties.type, kefir_ast_type_unsigned_long()));
    ASSERT(stmt2->tail != NULL);
    ASSERT(stmt2->tail->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt2->tail->properties.type, kefir_ast_type_unsigned_long_long()));
    ASSERT(stmt2->body->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT(context->resolve_ordinary_identifier(context, "x", &scoped_id) == KEFIR_NOT_FOUND);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_for_statements2, "AST node analysis - for statements #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "i"),
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x"))),
        NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));

    struct kefir_ast_declaration *decl3 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"),
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "i"))),
        NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl3->specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(decl3)));

    struct kefir_ast_for_statement *stmt1 = kefir_ast_new_for_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(decl2),
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_LESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "i")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "i")))),
        KEFIR_AST_NODE_BASE(compound1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt1->init->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, stmt1_init_decl_list, stmt1->init->self);
    ASSERT(kefir_list_length(&stmt1_init_decl_list->init_declarators) == 1);
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, stmt1_init_decl,
                     kefir_list_head(&stmt1_init_decl_list->init_declarators)->value);
    ASSERT(stmt1_init_decl->properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
    ASSERT(!stmt1_init_decl->properties.declaration_props.static_assertion);
    ASSERT(stmt1_init_decl->properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(stmt1_init_decl->properties.declaration_props.alignment == 0);
    ASSERT(strcmp(stmt1_init_decl->properties.declaration_props.identifier, "i") == 0);
    ASSERT(stmt1->controlling_expr->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt1->controlling_expr->properties.type, kefir_ast_type_signed_int()));
    ASSERT(stmt1->tail->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt1->tail->properties.type, kefir_ast_type_signed_int()));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(context->resolve_ordinary_identifier(context, "x", &scoped_id));
    ASSERT(context->resolve_ordinary_identifier(context, "i", &scoped_id) == KEFIR_NOT_FOUND);
    ASSERT(context->resolve_ordinary_identifier(context, "y", &scoped_id) == KEFIR_NOT_FOUND);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_for_statements3, "AST node analysis - for statements #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "i"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_storage_class_specifier_register(&kft_mem)));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(decl1))));

    struct kefir_ast_for_statement *stmt1 = kefir_ast_new_for_statement(
        &kft_mem, NULL, NULL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "i")))),
        KEFIR_AST_NODE_BASE(compound1));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));

    struct kefir_ast_compound_statement *compound2 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_for_statement *stmt2 = kefir_ast_new_for_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(decl1), NULL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "i")))),
        KEFIR_AST_NODE_BASE(compound2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt2->base.properties.statement_props.flow_control_point == NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.continuation != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->value.loop.end != NULL);
    ASSERT(stmt2->init->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, stmt2_init_decl_list, stmt2->init->self);
    ASSERT(kefir_list_length(&stmt2_init_decl_list->init_declarators) == 1);
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, stmt2_init_decl,
                     kefir_list_head(&stmt2_init_decl_list->init_declarators)->value);
    ASSERT(stmt2_init_decl->properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    ASSERT(!stmt2_init_decl->properties.declaration_props.static_assertion);
    ASSERT(stmt2_init_decl->properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(stmt2_init_decl->properties.declaration_props.alignment == 0);
    ASSERT(strcmp(stmt2_init_decl->properties.declaration_props.identifier, "i") == 0);
    ASSERT(stmt2->controlling_expr == NULL);
    ASSERT(stmt2->tail->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt2->tail->properties.type, kefir_ast_type_signed_long()));

    struct kefir_ast_declaration *decl3 = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "j"), NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl3->specifiers,
                                                         kefir_ast_storage_class_specifier_extern(&kft_mem)));

    struct kefir_ast_compound_statement *compound3 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_for_statement *stmt3 = kefir_ast_new_for_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(decl3), NULL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "j")))),
        KEFIR_AST_NODE_BASE(compound3));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
