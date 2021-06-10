#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

DEFINE_CASE(ast_nodes_static_assertions1, "AST nodes - static assertions #1")
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
    KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
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
END_CASE

DEFINE_CASE(ast_nodes_labeled_statements1, "AST nodes - labeled statements #1")
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
END_CASE

DEFINE_CASE(ast_nodes_case_statements1, "AST nodes - case statements #1")
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
END_CASE

DEFINE_CASE(ast_nodes_expression_statements1, "AST nodes - expression statements #1")
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
ASSERT(strcmp(((struct kefir_ast_string_literal *) stmt1->expression->self)->literal, "Goodbye, cruel world!") == 0);

struct kefir_ast_expression_statement *stmt2 =
    kefir_ast_new_expression_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 6.1987f)));
ASSERT(stmt2 != NULL);
ASSERT(stmt2->base.klass->type == KEFIR_AST_EXPRESSION_STATEMENT);
ASSERT(stmt2->base.self == stmt2);
ASSERT(stmt2->expression != NULL);
ASSERT(stmt2->expression->klass->type == KEFIR_AST_CONSTANT);
ASSERT(((struct kefir_ast_constant *) stmt2->expression->self)->type == KEFIR_AST_FLOAT_CONSTANT);
ASSERT(FLOAT_EQUALS(((struct kefir_ast_constant *) stmt2->expression->self)->value.float32, 6.1987f, FLOAT_EPSILON));

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
END_CASE
