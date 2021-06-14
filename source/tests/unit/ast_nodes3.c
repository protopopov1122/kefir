#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

DEFINE_CASE(ast_nodes_static_assertions1, "AST nodes - static assertions #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_static_assertion *assert1 =
        kefir_ast_new_static_assertion(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 123)),
                                       KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, goodbye!"));

    ASSERT(assert1 != NULL);
    ASSERT(assert1->base.klass->type == KEFIR_AST_STATIC_ASSERTION);
    ASSERT(assert1->base.self == assert1);
    ASSERT(assert1->condition != NULL);
    ASSERT(assert1->condition->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assert1->condition->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assert1->condition->self)->value.integer == 123);
    ASSERT(assert1->string != NULL);
    ASSERT(strcmp(assert1->string->literal, "Hello, goodbye!") == 0);

    struct kefir_ast_static_assertion *assert2 = kefir_ast_new_static_assertion(
        &kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_SUBTRACT, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)))),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Something"));

    ASSERT(assert2 != NULL);
    ASSERT(assert2->base.klass->type == KEFIR_AST_STATIC_ASSERTION);
    ASSERT(assert2->base.self == assert2);
    ASSERT(assert2->condition != NULL);
    ASSERT(assert2->condition->klass->type == KEFIR_AST_BINARY_OPERATION);
    ASSERT(assert2->string != NULL);
    ASSERT(strcmp(assert2->string->literal, "Something") == 0);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_labeled_statements1, "AST nodes - labeled statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_labeled_statement *stmt1 = kefir_ast_new_labeled_statement(
        &kft_mem, &symbols, "label1", KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_LABELED_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->label != NULL);
    ASSERT(strcmp(stmt1->label, "label1") == 0);
    ASSERT(stmt1->statement != NULL);
    ASSERT(stmt1->statement->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->statement->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->statement->self)->value.integer == 0);

    struct kefir_ast_labeled_statement *stmt2 = kefir_ast_new_labeled_statement(
        &kft_mem, &symbols, "label2", KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "label3")));
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->base.klass->type == KEFIR_AST_LABELED_STATEMENT);
    ASSERT(stmt2->base.self == stmt2);
    ASSERT(stmt2->label != NULL);
    ASSERT(strcmp(stmt2->label, "label2") == 0);
    ASSERT(stmt2->statement != NULL);
    ASSERT(stmt2->statement->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_identifier *) stmt2->statement->self)->identifier != NULL);
    ASSERT(strcmp(((struct kefir_ast_identifier *) stmt2->statement->self)->identifier, "label3") == 0);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_case_statements1, "AST nodes - case statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_case_statement *stmt1 =
        kefir_ast_new_case_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 1)),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "case1")));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_CASE_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->expression != NULL);
    ASSERT(stmt1->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->expression->self)->type == KEFIR_AST_LONG_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->expression->self)->value.integer == 1);
    ASSERT(stmt1->statement != NULL);
    ASSERT(stmt1->statement->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_identifier *) stmt1->statement->self)->identifier != NULL);
    ASSERT(strcmp(((struct kefir_ast_identifier *) stmt1->statement->self)->identifier, "case1") == 0);

    struct kefir_ast_case_statement *stmt2 =
        kefir_ast_new_case_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "case2")));
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->base.klass->type == KEFIR_AST_CASE_STATEMENT);
    ASSERT(stmt2->base.self == stmt2);
    ASSERT(stmt2->expression != NULL);
    ASSERT(stmt2->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt2->expression->self)->type == KEFIR_AST_CHAR_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt2->expression->self)->value.character == 'b');
    ASSERT(stmt2->statement != NULL);
    ASSERT(stmt2->statement->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_identifier *) stmt2->statement->self)->identifier != NULL);
    ASSERT(strcmp(((struct kefir_ast_identifier *) stmt2->statement->self)->identifier, "case2") == 0);

    struct kefir_ast_case_statement *stmt3 = kefir_ast_new_case_statement(
        &kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "case3")));
    ASSERT(stmt3 != NULL);
    ASSERT(stmt3->base.klass->type == KEFIR_AST_CASE_STATEMENT);
    ASSERT(stmt3->base.self == stmt3);
    ASSERT(stmt3->expression == NULL);
    ASSERT(stmt3->statement != NULL);
    ASSERT(stmt3->statement->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_identifier *) stmt3->statement->self)->identifier != NULL);
    ASSERT(strcmp(((struct kefir_ast_identifier *) stmt3->statement->self)->identifier, "case3") == 0);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_expression_statements1, "AST nodes - expression statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_expression_statement *stmt1 = kefir_ast_new_expression_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Goodbye, cruel world!")));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->expression != NULL);
    ASSERT(stmt1->expression->klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(((struct kefir_ast_string_literal *) stmt1->expression->self)->literal != NULL);
    ASSERT(strcmp(((struct kefir_ast_string_literal *) stmt1->expression->self)->literal, "Goodbye, cruel world!") ==
           0);

    struct kefir_ast_expression_statement *stmt2 = kefir_ast_new_expression_statement(
        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 6.1987f)));
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->base.klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(stmt2->base.self == stmt2);
    ASSERT(stmt2->expression != NULL);
    ASSERT(stmt2->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt2->expression->self)->type == KEFIR_AST_FLOAT_CONSTANT);
    ASSERT(
        FLOAT_EQUALS(((struct kefir_ast_constant *) stmt2->expression->self)->value.float32, 6.1987f, FLOAT_EPSILON));

    struct kefir_ast_expression_statement *stmt3 = kefir_ast_new_expression_statement(&kft_mem, NULL);
    ASSERT(stmt3 != NULL);
    ASSERT(stmt3->base.klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(stmt3->base.self == stmt3);
    ASSERT(stmt3->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_compound_statements1, "AST nodes - compound statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_compound_statement *stmt1 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_COMPOUND_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(kefir_list_length(&stmt1->block_items) == 0);

    ASSERT_OK(kefir_list_insert_after(&kft_mem, &stmt1->block_items, kefir_list_tail(&stmt1->block_items),
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                          &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, '1'))))));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &stmt1->block_items, kefir_list_tail(&stmt1->block_items),
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                          &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, '2'))))));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &stmt1->block_items, kefir_list_tail(&stmt1->block_items),
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                          &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, '3'))))));
    ASSERT(kefir_list_length(&stmt1->block_items) == 3);

    struct kefir_ast_compound_statement *stmt2 = kefir_ast_new_compound_statement(&kft_mem);
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->base.klass->type == KEFIR_AST_COMPOUND_STATEMENT);
    ASSERT(stmt2->base.self == stmt2);
    ASSERT(kefir_list_length(&stmt2->block_items) == 0);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_conditional_statements1, "AST nodes - conditional statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_conditional_statement *stmt1 =
        kefir_ast_new_conditional_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_CONDITIONAL_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->condition != NULL);
    ASSERT(stmt1->condition->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->condition->self)->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(stmt1->thenBranch != NULL);
    ASSERT(stmt1->thenBranch->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->thenBranch->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(stmt1->elseBranch != NULL);
    ASSERT(stmt1->elseBranch->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->elseBranch->self)->type == KEFIR_AST_DOUBLE_CONSTANT);

    struct kefir_ast_conditional_statement *stmt2 =
        kefir_ast_new_conditional_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 1.0f)),
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 10)), NULL);
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->base.klass->type == KEFIR_AST_CONDITIONAL_STATEMENT);
    ASSERT(stmt2->base.self == stmt2);
    ASSERT(stmt2->condition != NULL);
    ASSERT(stmt2->condition->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt2->condition->self)->type == KEFIR_AST_FLOAT_CONSTANT);
    ASSERT(stmt2->thenBranch != NULL);
    ASSERT(stmt2->thenBranch->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt2->thenBranch->self)->type == KEFIR_AST_LONG_CONSTANT);
    ASSERT(stmt2->elseBranch == NULL);

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, ' '));
    struct kefir_ast_conditional_statement *stmt3 = kefir_ast_new_conditional_statement(&kft_mem, node1, NULL, NULL);
    ASSERT(stmt3 == NULL);

    struct kefir_ast_conditional_statement *stmt4 = kefir_ast_new_conditional_statement(&kft_mem, NULL, node1, NULL);
    ASSERT(stmt4 == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_switch_statements1, "AST nodes - switch statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_switch_statement *stmt1 =
        kefir_ast_new_switch_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_SWITCH_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->expression != NULL);
    ASSERT(stmt1->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->expression->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->expression->self)->value.integer == 1);
    ASSERT(stmt1->statement != NULL);
    ASSERT(stmt1->statement->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt1->statement->self)->expression == NULL);

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1));
    ASSERT(node1 != NULL);
    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL));
    ASSERT(node2 != NULL);

    struct kefir_ast_switch_statement *stmt2 = kefir_ast_new_switch_statement(&kft_mem, node1, NULL);
    ASSERT(stmt2 == NULL);

    struct kefir_ast_switch_statement *stmt3 = kefir_ast_new_switch_statement(&kft_mem, NULL, node2);
    ASSERT(stmt3 == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node2));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_while_statements1, "AST nodes - while statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_while_statement *stmt1 =
        kefir_ast_new_while_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                      KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_WHILE_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->controlling_expr != NULL);
    ASSERT(stmt1->controlling_expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->value.boolean);
    ASSERT(stmt1->body != NULL);
    ASSERT(stmt1->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt1->body->self)->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_while_statements2, "AST nodes - while statements #2") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false));
    struct kefir_ast_while_statement *stmt1 = kefir_ast_new_while_statement(&kft_mem, node1, NULL);
    ASSERT(stmt1 == NULL);

    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL));
    struct kefir_ast_while_statement *stmt2 = kefir_ast_new_while_statement(&kft_mem, NULL, node2);
    ASSERT(stmt2 == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node2));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_do_while_statements1, "AST nodes - do while statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_do_while_statement *stmt1 =
        kefir_ast_new_do_while_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                         KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_DO_WHILE_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->controlling_expr != NULL);
    ASSERT(stmt1->controlling_expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->value.boolean);
    ASSERT(stmt1->body != NULL);
    ASSERT(stmt1->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt1->body->self)->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_do_while_statements2, "AST nodes - do while statements #2") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false));
    struct kefir_ast_do_while_statement *stmt1 = kefir_ast_new_do_while_statement(&kft_mem, node1, NULL);
    ASSERT(stmt1 == NULL);

    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL));
    struct kefir_ast_do_while_statement *stmt2 = kefir_ast_new_do_while_statement(&kft_mem, NULL, node2);
    ASSERT(stmt2 == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node2));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_for_statements1, "AST nodes - for statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_for_statement *stmt1 =
        kefir_ast_new_for_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_FOR_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->init != NULL);
    ASSERT(stmt1->init->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->init->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->init->self)->value.integer == 0);
    ASSERT(stmt1->controlling_expr != NULL);
    ASSERT(stmt1->controlling_expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->value.integer == 1);
    ASSERT(stmt1->tail != NULL);
    ASSERT(stmt1->tail->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->tail->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->tail->self)->value.integer == 2);
    ASSERT(stmt1->body != NULL);
    ASSERT(stmt1->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt1->body->self)->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_for_statements2, "AST nodes - for statements #2") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_for_statement *stmt1 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_FOR_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->init == NULL);
    ASSERT(stmt1->controlling_expr != NULL);
    ASSERT(stmt1->controlling_expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->value.integer == 1);
    ASSERT(stmt1->tail != NULL);
    ASSERT(stmt1->tail->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->tail->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->tail->self)->value.integer == 2);
    ASSERT(stmt1->body != NULL);
    ASSERT(stmt1->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt1->body->self)->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_for_statements3, "AST nodes - for statements #3") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_for_statement *stmt1 =
        kefir_ast_new_for_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), NULL,
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_FOR_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->init != NULL);
    ASSERT(stmt1->init->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->init->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->init->self)->value.integer == 0);
    ASSERT(stmt1->controlling_expr == NULL);
    ASSERT(stmt1->tail != NULL);
    ASSERT(stmt1->tail->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->tail->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->tail->self)->value.integer == 2);
    ASSERT(stmt1->body != NULL);
    ASSERT(stmt1->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt1->body->self)->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_for_statements4, "AST nodes - for statements #4") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_for_statement *stmt1 =
        kefir_ast_new_for_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)), NULL,
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt1 != NULL);
    ASSERT(stmt1->base.klass->type == KEFIR_AST_FOR_STATEMENT);
    ASSERT(stmt1->base.self == stmt1);
    ASSERT(stmt1->init != NULL);
    ASSERT(stmt1->init->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->init->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->init->self)->value.integer == 0);
    ASSERT(stmt1->controlling_expr != NULL);
    ASSERT(stmt1->controlling_expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt1->controlling_expr->self)->value.integer == 1);
    ASSERT(stmt1->tail == NULL);
    ASSERT(stmt1->body != NULL);
    ASSERT(stmt1->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt1->body->self)->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_for_statements5, "AST nodes - for statements #5") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_node_base *node1 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0));
    struct kefir_ast_node_base *node2 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1));
    struct kefir_ast_node_base *node3 = KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2));

    struct kefir_ast_for_statement *stmt1 = kefir_ast_new_for_statement(&kft_mem, node1, node2, node3, NULL);
    ASSERT(stmt1 == NULL);

    struct kefir_ast_for_statement *stmt2 =
        kefir_ast_new_for_statement(&kft_mem, NULL, NULL, KEFIR_AST_NODE_CLONE(&kft_mem, node3),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt2 != NULL);
    ASSERT(stmt2->init == NULL);
    ASSERT(stmt2->controlling_expr == NULL);
    ASSERT(stmt2->tail != NULL);
    ASSERT(stmt2->tail->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt2->tail->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt2->tail->self)->value.integer == 2);
    ASSERT(stmt2->body != NULL);
    ASSERT(stmt2->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt2->body->self)->expression == NULL);

    struct kefir_ast_for_statement *stmt3 =
        kefir_ast_new_for_statement(&kft_mem, NULL, KEFIR_AST_NODE_CLONE(&kft_mem, node2), NULL,
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt3 != NULL);
    ASSERT(stmt3->init == NULL);
    ASSERT(stmt3->controlling_expr != NULL);
    ASSERT(stmt3->controlling_expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt3->controlling_expr->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt3->controlling_expr->self)->value.integer == 1);
    ASSERT(stmt3->tail == NULL);
    ASSERT(stmt3->body != NULL);
    ASSERT(stmt3->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt3->body->self)->expression == NULL);

    struct kefir_ast_for_statement *stmt4 =
        kefir_ast_new_for_statement(&kft_mem, KEFIR_AST_NODE_CLONE(&kft_mem, node1), NULL, NULL,
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt4 != NULL);
    ASSERT(stmt4->init != NULL);
    ASSERT(stmt4->init->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt4->init->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) stmt4->init->self)->value.integer == 0);
    ASSERT(stmt4->controlling_expr == NULL);
    ASSERT(stmt4->tail == NULL);
    ASSERT(stmt4->body != NULL);
    ASSERT(stmt4->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt4->body->self)->expression == NULL);

    struct kefir_ast_for_statement *stmt5 = kefir_ast_new_for_statement(
        &kft_mem, NULL, NULL, NULL, KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(&kft_mem, NULL)));
    ASSERT(stmt5 != NULL);
    ASSERT(stmt5->init == NULL);
    ASSERT(stmt5->controlling_expr == NULL);
    ASSERT(stmt5->tail == NULL);
    ASSERT(stmt5->body != NULL);
    ASSERT(stmt5->body->klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
    ASSERT(((struct kefir_ast_expression_statement *) stmt5->body->self)->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node1));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node2));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, node3));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt4)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt5)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
