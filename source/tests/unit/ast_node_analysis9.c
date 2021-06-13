#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/function_declaration_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_node_analysis_conditinal_statements1, "AST node analysis - conditional statements #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_conditional_statement *stmt1 = kefir_ast_new_conditional_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)), NULL);
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt1->condition->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt1->condition->properties.type, kefir_ast_type_bool()));
    ASSERT(stmt1->thenBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->elseBranch == NULL);

    struct kefir_ast_conditional_statement *stmt2 = kefir_ast_new_conditional_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, '\b')))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -1)))));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt2->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt2->condition->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt2->condition->properties.type, kefir_ast_type_bool()));
    ASSERT(stmt2->thenBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt2->elseBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_conditinal_statements2, "AST node analysis - conditional statements #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(
        &kft_mem,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x")),
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(&kft_mem);
    struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"),
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2->specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                      KEFIR_AST_NODE_BASE(decl2)));

    struct kefir_ast_compound_statement *compound2 = kefir_ast_new_compound_statement(&kft_mem);
    struct kefir_ast_declaration *decl3 = kefir_ast_new_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"),
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl3->specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &compound2->block_items, kefir_list_tail(&compound2->block_items),
                                      KEFIR_AST_NODE_BASE(decl3)));

    struct kefir_ast_conditional_statement *stmt1 = kefir_ast_new_conditional_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
        KEFIR_AST_NODE_BASE(compound1), KEFIR_AST_NODE_BASE(compound2));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));

    ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt1->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt1->condition->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt1->condition->properties.type,
                               kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void())));
    ASSERT(stmt1->thenBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt1->elseBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_conditinal_statements3, "AST node analysis - conditional statements #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_conditional_statement *stmt1 = kefir_ast_new_conditional_statement(
        &kft_mem,
        KEFIR_AST_NODE_BASE(
            kefir_ast_new_expression_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                             kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));

    struct kefir_ast_compound_literal *compound1 = kefir_ast_new_compound_literal(&kft_mem, type_name1);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &compound1->initializer->list, NULL,
        kefir_ast_new_expression_initializer(
            &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Something")))));

    struct kefir_ast_conditional_statement *stmt2 =
        kefir_ast_new_conditional_statement(&kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(compound1)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));

    struct kefir_ast_conditional_statement *stmt3 =
        kefir_ast_new_conditional_statement(&kft_mem,
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                                                &kft_mem, KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(compound1))),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT(stmt3->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt3->base.properties.statement_props.flow_control_statement != NULL);
    ASSERT(stmt3->base.properties.statement_props.flow_control_statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_IF);
    ASSERT(stmt3->condition->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(stmt3->condition->properties.type, kefir_ast_type_signed_int()));
    ASSERT(stmt3->thenBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
    ASSERT(stmt3->elseBranch->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    struct kefir_ast_conditional_statement *stmt4 =
        kefir_ast_new_conditional_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt4)));

    struct kefir_ast_conditional_statement *stmt5 =
        kefir_ast_new_conditional_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')));
    ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt5)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt4)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt5)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
