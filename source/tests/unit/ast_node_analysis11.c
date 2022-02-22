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

DEFINE_CASE(ast_node_analysis_goto_statements1, "AST node analysis - goto statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_goto_statement *goto1 = kefir_ast_new_goto_statement(&kft_mem, context->symbols, "label4");
    struct kefir_ast_labeled_statement *labeled1 =
        kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label1", KEFIR_AST_NODE_BASE(goto1));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), labeled1));

    struct kefir_ast_goto_statement *goto2 = kefir_ast_new_goto_statement(&kft_mem, context->symbols, "label3");
    struct kefir_ast_labeled_statement *labeled2 =
        kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label2", KEFIR_AST_NODE_BASE(goto2));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), labeled2));

    struct kefir_ast_goto_statement *goto3 = kefir_ast_new_goto_statement(&kft_mem, context->symbols, "label2");
    struct kefir_ast_labeled_statement *labeled3 =
        kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label3", KEFIR_AST_NODE_BASE(goto3));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), labeled3));

    struct kefir_ast_goto_statement *goto4 = kefir_ast_new_goto_statement(&kft_mem, context->symbols, "label1");
    struct kefir_ast_labeled_statement *labeled4 =
        kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label4", KEFIR_AST_NODE_BASE(goto4));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), labeled4));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(compound1)));

    ASSERT(goto1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(labeled4->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(goto1->base.properties.statement_props.target_flow_control_point ==
           labeled4->base.properties.statement_props.target_flow_control_point);
    ASSERT(goto1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(goto1->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);
    ASSERT(labeled4->base.properties.statement_props.flow_control_statement == NULL);

    ASSERT(goto2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(labeled3->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(goto2->base.properties.statement_props.target_flow_control_point ==
           labeled3->base.properties.statement_props.target_flow_control_point);
    ASSERT(goto2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(goto2->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);
    ASSERT(labeled3->base.properties.statement_props.flow_control_statement == NULL);

    ASSERT(goto3->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(labeled2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(goto3->base.properties.statement_props.target_flow_control_point ==
           labeled2->base.properties.statement_props.target_flow_control_point);
    ASSERT(goto3->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(goto3->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);
    ASSERT(labeled2->base.properties.statement_props.flow_control_statement == NULL);

    ASSERT(goto4->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(labeled1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(goto4->base.properties.statement_props.target_flow_control_point ==
           labeled1->base.properties.statement_props.target_flow_control_point);
    ASSERT(goto4->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(goto4->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);
    ASSERT(labeled1->base.properties.statement_props.flow_control_statement == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(compound1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_goto_address_statements1, "AST node analysis - goto address statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_scoped_identifier *scoped1 = NULL;
    ASSERT_OK(context->define_identifier(&kft_mem, context, true, "label1",
                                         kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
                                         KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                         NULL, NULL, NULL, &scoped1));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_goto_statement *goto1 = kefir_ast_new_goto_address_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                      &kft_mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(goto1)));

    struct kefir_ast_goto_statement *goto2 = kefir_ast_new_goto_address_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "string literal")));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(goto2)));

    struct kefir_ast_goto_statement *goto3 = kefir_ast_new_goto_address_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "label1")));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(goto3)));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(compound1)));

    ASSERT(goto1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(goto1->base.properties.statement_props.target_flow_control_point == NULL);
    ASSERT(goto1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(goto1->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);

    ASSERT(goto2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(goto2->base.properties.statement_props.target_flow_control_point == NULL);
    ASSERT(goto2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(goto2->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);

    ASSERT(goto3->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(goto3->base.properties.statement_props.target_flow_control_point == NULL);
    ASSERT(goto3->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(goto3->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(compound1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_break_statements1, "AST node analysis - break statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_while_statement *while1 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(break1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(while1)));

    ASSERT(break1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(while1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break1->base.properties.statement_props.target_flow_control_point ==
           while1->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point->parent ==
           while1->base.properties.statement_props.flow_control_statement);

    struct kefir_ast_break_statement *break2 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_while_statement *while2 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(break2));
    struct kefir_ast_for_statement *for3 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                    NULL, KEFIR_AST_NODE_BASE(while2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(for3)));

    ASSERT(break2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(while2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point ==
           while2->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent ==
           while2->base.properties.statement_props.flow_control_statement);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point !=
           for3->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent !=
           for3->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(while1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(for3)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_break_statements2, "AST node analysis - break statements #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_do_while_statement *do_while1 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(break1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(do_while1)));

    ASSERT(break1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(do_while1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break1->base.properties.statement_props.target_flow_control_point ==
           do_while1->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point->parent ==
           do_while1->base.properties.statement_props.flow_control_statement);

    struct kefir_ast_break_statement *break2 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_do_while_statement *do_while2 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(break2));
    struct kefir_ast_while_statement *while3 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(do_while2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(while3)));

    ASSERT(break2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(do_while2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point ==
           do_while2->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent ==
           do_while2->base.properties.statement_props.flow_control_statement);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point !=
           while3->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent !=
           while3->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(do_while1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(while3)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_break_statements3, "AST node analysis - break statements #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_for_statement *for1 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                    NULL, KEFIR_AST_NODE_BASE(break1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(for1)));

    ASSERT(break1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(for1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break1->base.properties.statement_props.target_flow_control_point ==
           for1->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point->parent ==
           for1->base.properties.statement_props.flow_control_statement);

    struct kefir_ast_break_statement *break2 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_for_statement *for2 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                    NULL, KEFIR_AST_NODE_BASE(break2));
    struct kefir_ast_do_while_statement *do_while3 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(for2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(do_while3)));

    ASSERT(break2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(for2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point ==
           for2->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent ==
           for2->base.properties.statement_props.flow_control_statement);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point !=
           do_while3->base.properties.statement_props.flow_control_statement->value.loop.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent !=
           do_while3->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(for1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(do_while3)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_break_statements4, "AST node analysis - break statements #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_case_statement *case1 = kefir_ast_new_case_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), KEFIR_AST_NODE_BASE(break1));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(case1)));

    struct kefir_ast_break_statement *break2 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_case_statement *case2 = kefir_ast_new_case_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)), KEFIR_AST_NODE_BASE(break2));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(case2)));

    struct kefir_ast_break_statement *break3 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_case_statement *case3 = kefir_ast_new_case_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(break3));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(case3)));

    struct kefir_ast_switch_statement *switch1 = kefir_ast_new_switch_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)), KEFIR_AST_NODE_BASE(compound1));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(switch1)));

    ASSERT(break1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(switch1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break1->base.properties.statement_props.target_flow_control_point ==
           switch1->base.properties.statement_props.flow_control_statement->value.switchStatement.end);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);

    ASSERT(break2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(switch1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point ==
           switch1->base.properties.statement_props.flow_control_statement->value.switchStatement.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);

    ASSERT(break3->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(switch1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break3->base.properties.statement_props.target_flow_control_point ==
           switch1->base.properties.statement_props.flow_control_statement->value.switchStatement.end);
    ASSERT(break3->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break3->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(switch1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_break_statements5, "AST node analysis - break statements #5") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_case_statement *case1 = kefir_ast_new_case_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), KEFIR_AST_NODE_BASE(break1));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case1));

    struct kefir_ast_switch_statement *switch1 = kefir_ast_new_switch_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)), KEFIR_AST_NODE_BASE(compound1));

    struct kefir_ast_compound_statement *compound2 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_break_statement *break2 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_case_statement *case2 = kefir_ast_new_case_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -1)), KEFIR_AST_NODE_BASE(switch1));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
                                      KEFIR_AST_NODE_BASE(case2)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
                                      KEFIR_AST_NODE_BASE(break2)));

    struct kefir_ast_switch_statement *switch2 = kefir_ast_new_switch_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)), KEFIR_AST_NODE_BASE(compound2));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(switch2)));

    ASSERT(break1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(break2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(switch1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(switch2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    ASSERT(break1->base.properties.statement_props.target_flow_control_point ==
           switch1->base.properties.statement_props.flow_control_statement->value.switchStatement.end);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point->parent ==
           compound1->base.properties.statement_props.flow_control_statement);
    ASSERT(break1->base.properties.statement_props.target_flow_control_point !=
           switch2->base.properties.statement_props.flow_control_statement->value.switchStatement.end);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break1->base.properties.statement_props.origin_flow_control_point->parent !=
           switch2->base.properties.statement_props.flow_control_statement);

    ASSERT(break2->base.properties.statement_props.target_flow_control_point ==
           switch2->base.properties.statement_props.flow_control_statement->value.switchStatement.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent ==
           compound2->base.properties.statement_props.flow_control_statement);
    ASSERT(break2->base.properties.statement_props.target_flow_control_point !=
           switch1->base.properties.statement_props.flow_control_statement->value.switchStatement.end);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(break2->base.properties.statement_props.origin_flow_control_point->parent !=
           switch1->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(switch2)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_break_statements6, "AST node analysis - break statements #6") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    struct kefir_ast_conditional_statement *condition1 = kefir_ast_new_conditional_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)), KEFIR_AST_NODE_BASE(break1), NULL);

    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(condition1)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(condition1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_continue_statements1, "AST node analysis - continue statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_continue_statement *continue1 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_while_statement *while1 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(continue1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(while1)));

    ASSERT(continue1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(while1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(continue1->base.properties.statement_props.target_flow_control_point ==
           while1->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue1->base.properties.statement_props.origin_flow_control_point->parent ==
           while1->base.properties.statement_props.flow_control_statement);

    struct kefir_ast_continue_statement *continue2 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_while_statement *while2 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(continue2));
    struct kefir_ast_for_statement *for3 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                    NULL, KEFIR_AST_NODE_BASE(while2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(for3)));

    ASSERT(continue2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(while2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(continue2->base.properties.statement_props.target_flow_control_point ==
           while2->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point->parent ==
           while2->base.properties.statement_props.flow_control_statement);
    ASSERT(continue2->base.properties.statement_props.target_flow_control_point !=
           for3->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point->parent !=
           for3->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(while1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(for3)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_continue_statements2, "AST node analysis - continue statements #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_continue_statement *continue1 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_do_while_statement *do_while1 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(continue1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(do_while1)));

    ASSERT(continue1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(do_while1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(continue1->base.properties.statement_props.target_flow_control_point ==
           do_while1->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue1->base.properties.statement_props.origin_flow_control_point->parent != NULL);
    ASSERT(continue1->base.properties.statement_props.origin_flow_control_point->parent ==
           do_while1->base.properties.statement_props.flow_control_statement);

    struct kefir_ast_continue_statement *continue2 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_do_while_statement *do_while2 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(continue2));
    struct kefir_ast_while_statement *while3 = kefir_ast_new_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(do_while2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(while3)));

    ASSERT(continue2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(do_while2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(continue2->base.properties.statement_props.target_flow_control_point ==
           do_while2->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point->parent ==
           do_while2->base.properties.statement_props.flow_control_statement);
    ASSERT(continue2->base.properties.statement_props.target_flow_control_point !=
           while3->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point->parent !=
           while3->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(do_while1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(while3)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_continue_statements3, "AST node analysis - continue statements #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_continue_statement *continue1 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_for_statement *for1 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                    NULL, KEFIR_AST_NODE_BASE(continue1));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(for1)));

    ASSERT(continue1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(for1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(continue1->base.properties.statement_props.target_flow_control_point ==
           for1->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue1->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue1->base.properties.statement_props.origin_flow_control_point->parent ==
           for1->base.properties.statement_props.flow_control_statement);

    struct kefir_ast_continue_statement *continue2 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_for_statement *for2 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                    NULL, KEFIR_AST_NODE_BASE(continue2));
    struct kefir_ast_do_while_statement *do_while3 = kefir_ast_new_do_while_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)), KEFIR_AST_NODE_BASE(for2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(do_while3)));

    ASSERT(continue2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(for2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(continue2->base.properties.statement_props.target_flow_control_point ==
           for2->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point->parent ==
           for2->base.properties.statement_props.flow_control_statement);
    ASSERT(continue2->base.properties.statement_props.target_flow_control_point !=
           do_while3->base.properties.statement_props.flow_control_statement->value.loop.continuation);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point != NULL);
    ASSERT(continue2->base.properties.statement_props.origin_flow_control_point->parent !=
           do_while3->base.properties.statement_props.flow_control_statement);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(for1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(do_while3)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_continue_statements4, "AST node analysis - continue statements #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);

    struct kefir_ast_continue_statement *continue1 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_case_statement *case1 = kefir_ast_new_case_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), KEFIR_AST_NODE_BASE(continue1));
    ASSERT_OK(
        kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case1));

    struct kefir_ast_switch_statement *switch1 = kefir_ast_new_switch_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)), KEFIR_AST_NODE_BASE(compound1));

    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(switch1)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(switch1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_continue_statements5, "AST node analysis - continue statements #5") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_continue_statement *continue1 = kefir_ast_new_continue_statement(&kft_mem);
    struct kefir_ast_conditional_statement *condition1 =
        kefir_ast_new_conditional_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
                                            KEFIR_AST_NODE_BASE(continue1), NULL);

    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(condition1)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(condition1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_return_statements1, "AST node analysis - return statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function_type = NULL;
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &function_type);
    REQUIRE_OK(global_context.context.define_identifier(
        &kft_mem, &global_context.context, true, "fn0", type1, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, &scoped_id));
    local_context.context.surrounding_function = scoped_id;

    struct kefir_ast_return_statement *return1 = kefir_ast_new_return_statement(&kft_mem, NULL);
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(return1)));
    ASSERT(return1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(return1->base.properties.statement_props.target_flow_control_point == NULL);
    ASSERT(return1->base.properties.statement_props.origin_flow_control_point == NULL);
    ASSERT(return1->base.properties.statement_props.flow_control_statement == NULL);
    ASSERT(return1->expression == NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(return1->base.properties.statement_props.return_type, kefir_ast_type_void()));

    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(), &function_type);
    REQUIRE_OK(global_context.context.define_identifier(
        &kft_mem, &global_context.context, true, "fn1", type2, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, &scoped_id));
    local_context.context.surrounding_function = scoped_id;

    struct kefir_ast_return_statement *return2 =
        kefir_ast_new_return_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 1000)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(return2)));
    ASSERT(return2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(return2->base.properties.statement_props.target_flow_control_point == NULL);
    ASSERT(return2->base.properties.statement_props.origin_flow_control_point == NULL);
    ASSERT(return2->base.properties.statement_props.flow_control_statement == NULL);
    ASSERT(return2->expression != NULL);
    ASSERT(return2->expression->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(return2->expression->properties.type, kefir_ast_type_signed_long()));
    ASSERT(KEFIR_AST_TYPE_SAME(return2->base.properties.statement_props.return_type, kefir_ast_type_unsigned_short()));

    struct kefir_ast_return_statement *return3 = kefir_ast_new_return_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(return3)));

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));

    struct kefir_ast_return_statement *return4 =
        kefir_ast_new_return_statement(&kft_mem, KEFIR_AST_NODE_BASE(type_name1));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(return4)));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        &kft_mem,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "voidptr")),
        NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_return_statement *return5 = kefir_ast_new_return_statement(&kft_mem, KEFIR_AST_NODE_BASE(decl1));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(return5)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return3)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return4)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return5)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
