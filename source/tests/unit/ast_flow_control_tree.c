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

#include "kefir/test/unit_test.h"
#include "kefir/ast/flow_control.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/function_declaration_context.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_flow_control_tree1, "AST Flow control tree - global context") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));

    ASSERT(global_context.context.flow_control_tree == NULL);

    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_flow_control_tree2, "AST Flow control tree - function declaration context") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_function_declaration_context func_decl_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_function_declaration_context_init(&kft_mem, &global_context.context, &func_decl_context));

    ASSERT(func_decl_context.context.flow_control_tree == NULL);

    ASSERT_OK(kefir_ast_function_declaration_context_free(&kft_mem, &func_decl_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_flow_control_tree3, "AST Flow control tree - local context #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT(context->flow_control_tree != NULL);

    struct kefir_ast_flow_control_statement *stmt = NULL;
    struct kefir_ast_flow_control_statement *stmt2 = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt2 == NULL);

    ASSERT_NOK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_IF, &stmt));
    ASSERT(stmt != NULL);
    ASSERT(stmt->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt->parent == NULL);
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR, &stmt));
    ASSERT(stmt != NULL);
    ASSERT(stmt->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt->parent != NULL);
    ASSERT(stmt->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt->parent->parent == NULL);
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    ASSERT(stmt != NULL);
    ASSERT(stmt->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);
    ASSERT(stmt->parent != NULL);
    ASSERT(stmt->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt->parent->parent != NULL);
    ASSERT(stmt->parent->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt->parent->parent->parent == NULL);
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt2->parent != NULL);
    ASSERT(stmt2->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt2->parent->parent == NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_DO, &stmt));
    ASSERT(stmt != NULL);
    ASSERT(stmt->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_DO);
    ASSERT(stmt->parent != NULL);
    ASSERT(stmt->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR);
    ASSERT(stmt->parent->parent != NULL);
    ASSERT(stmt->parent->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt->parent->parent->parent == NULL);
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt2->parent == NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE, &stmt));
    ASSERT(stmt != NULL);
    ASSERT(stmt->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);
    ASSERT(stmt->parent != NULL);
    ASSERT(stmt->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);
    ASSERT(stmt->parent->parent != NULL);
    ASSERT(stmt->parent->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt->parent->parent->parent == NULL);
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_NOK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt2 == NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_DO, &stmt));
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_IF, &stmt));
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE, &stmt));

    ASSERT(stmt != NULL);
    ASSERT(stmt->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);
    ASSERT(stmt->parent != NULL);
    ASSERT(stmt->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);
    ASSERT(stmt->parent->parent != NULL);
    ASSERT(stmt->parent->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt->parent->parent->parent != NULL);
    ASSERT(stmt->parent->parent->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_DO);
    ASSERT(stmt->parent->parent->parent->parent == NULL);
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_NOK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt2 == NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    ASSERT(stmt->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);
    ASSERT(stmt->parent == NULL);
    ASSERT_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &stmt2));
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

static kefir_result_t test_free(struct kefir_mem *mem, struct kefir_ast_flow_control_statement *stmt, void *payload) {
    UNUSED(payload);
    ASSERT(mem != NULL);
    ASSERT(stmt != NULL);
    KEFIR_FREE(mem, *((void **) stmt->payload.ptr));
    return KEFIR_OK;
}

DEFINE_CASE(ast_flow_control_tree4, "AST Flow control tree - local context #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT(context->flow_control_tree != NULL);

    struct kefir_ast_flow_control_statement *stmt = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_DO, &stmt));
    *((void **) stmt->payload.ptr) = KEFIR_MALLOC(&kft_mem, 100);
    KEFIR_AST_FLOW_CONTROL_SET_CLEANUP(stmt, test_free, NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    *((void **) stmt->payload.ptr) = KEFIR_MALLOC(&kft_mem, 101);
    KEFIR_AST_FLOW_CONTROL_SET_CLEANUP(stmt, test_free, NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_IF, &stmt));
    *((void **) stmt->payload.ptr) = KEFIR_MALLOC(&kft_mem, 99);
    KEFIR_AST_FLOW_CONTROL_SET_CLEANUP(stmt, test_free, NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    *((void **) stmt->payload.ptr) = KEFIR_MALLOC(&kft_mem, 98);
    KEFIR_AST_FLOW_CONTROL_SET_CLEANUP(stmt, test_free, NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR, &stmt));
    *((void **) stmt->payload.ptr) = KEFIR_MALLOC(&kft_mem, 200);
    KEFIR_AST_FLOW_CONTROL_SET_CLEANUP(stmt, test_free, NULL);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

static kefir_result_t find_statement(const struct kefir_ast_flow_control_statement *stmt, void *payload,
                                     kefir_bool_t *result) {
    ASSERT(stmt != NULL);
    ASSERT(payload != NULL);
    ASSERT(result != NULL);

    *result = stmt->type == *(kefir_ast_flow_control_statement_type_t *) payload;
    return KEFIR_OK;
}

static kefir_result_t find_statement2(const struct kefir_ast_flow_control_statement *stmt, void *payload,
                                      kefir_bool_t *result) {
    ASSERT(stmt != NULL);
    ASSERT(payload == NULL);
    ASSERT(result != NULL);

    *result = true;
    return KEFIR_OK;
}

DEFINE_CASE(ast_flow_control_tree5, "AST Flow control tree - local context #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT(context->flow_control_tree != NULL);

    struct kefir_ast_flow_control_statement *stmt = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_DO, &stmt));
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE, &stmt));
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_IF, &stmt));

    struct kefir_ast_flow_control_statement *stmt2 = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_traverse(
        context->flow_control_tree, find_statement,
        &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_IF}, &stmt2));
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt2->parent != NULL);
    ASSERT(stmt2->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);
    ASSERT(stmt == stmt2);

    ASSERT_OK(kefir_ast_flow_control_tree_traverse(
        context->flow_control_tree, find_statement,
        &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE}, &stmt2));
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);
    ASSERT(stmt2->parent != NULL);
    ASSERT(stmt2->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_DO);

    ASSERT_OK(kefir_ast_flow_control_tree_traverse(
        context->flow_control_tree, find_statement,
        &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_DO}, &stmt2));
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_DO);
    ASSERT(stmt2->parent == NULL);

    ASSERT(kefir_ast_flow_control_tree_traverse(
               context->flow_control_tree, find_statement,
               &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH},
               &stmt2) == KEFIR_NOT_FOUND);
    ASSERT(kefir_ast_flow_control_tree_traverse(
               context->flow_control_tree, find_statement,
               &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR},
               &stmt2) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_traverse(
        context->flow_control_tree, find_statement,
        &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE}, &stmt2));
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);
    ASSERT(stmt2->parent != NULL);
    ASSERT(stmt2->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_DO);

    ASSERT(kefir_ast_flow_control_tree_traverse(
               context->flow_control_tree, find_statement,
               &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_IF},
               &stmt2) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt));

    ASSERT_OK(kefir_ast_flow_control_tree_traverse(
        context->flow_control_tree, find_statement,
        &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH}, &stmt2));
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);
    ASSERT(stmt2->parent != NULL);
    ASSERT(stmt2->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_traverse(
        context->flow_control_tree, find_statement,
        &(kefir_ast_flow_control_statement_type_t){KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH}, &stmt2));
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);
    ASSERT(stmt2->parent != NULL);
    ASSERT(stmt2->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);

    ASSERT_OK(kefir_ast_flow_control_tree_traverse(context->flow_control_tree, find_statement2, NULL, &stmt2));
    ASSERT(stmt2->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH);
    ASSERT(stmt2->parent != NULL);
    ASSERT(stmt2->parent->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE);

    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));
    ASSERT_NOK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    ASSERT(kefir_ast_flow_control_tree_traverse(context->flow_control_tree, find_statement2, NULL, &stmt2) ==
           KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_flow_control_tree6, "AST Flow control tree - flow control value #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_flow_control_statement *stmt1 = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_FOR, &stmt1));
    stmt1->value.loop.continuation = kefir_ast_flow_control_point_alloc(&kft_mem);
    stmt1->value.loop.end = kefir_ast_flow_control_point_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    struct kefir_ast_flow_control_statement *stmt2 = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_DO, &stmt2));
    stmt2->value.loop.continuation = kefir_ast_flow_control_point_alloc(&kft_mem);
    stmt2->value.loop.end = kefir_ast_flow_control_point_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    struct kefir_ast_flow_control_statement *stmt3 = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_WHILE, &stmt3));
    stmt3->value.loop.continuation = kefir_ast_flow_control_point_alloc(&kft_mem);
    stmt3->value.loop.end = kefir_ast_flow_control_point_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    struct kefir_ast_flow_control_statement *stmt4 = NULL;
    ASSERT_OK(kefir_ast_flow_control_tree_push(&kft_mem, context->flow_control_tree,
                                               KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH, &stmt4));
    ASSERT_OK(kefir_hashtree_insert(&kft_mem, &stmt4->value.switchStatement.cases, (kefir_hashtree_key_t) -1,
                                    (kefir_hashtree_value_t) kefir_ast_flow_control_point_alloc(&kft_mem)));
    ASSERT_OK(kefir_hashtree_insert(&kft_mem, &stmt4->value.switchStatement.cases, (kefir_hashtree_key_t) 0,
                                    (kefir_hashtree_value_t) kefir_ast_flow_control_point_alloc(&kft_mem)));
    ASSERT_OK(kefir_hashtree_insert(&kft_mem, &stmt4->value.switchStatement.cases, (kefir_hashtree_key_t) 1,
                                    (kefir_hashtree_value_t) kefir_ast_flow_control_point_alloc(&kft_mem)));
    ASSERT_OK(kefir_hashtree_insert(&kft_mem, &stmt4->value.switchStatement.cases, (kefir_hashtree_key_t) 0xfff,
                                    (kefir_hashtree_value_t) kefir_ast_flow_control_point_alloc(&kft_mem)));
    stmt4->value.switchStatement.defaultCase = kefir_ast_flow_control_point_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_flow_control_tree_pop(context->flow_control_tree));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
