#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_node_analysis_static_assertions1, "AST node analysis - static assertions #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "X",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 1),
                                                  context->type_traits->underlying_enumeration_type, NULL));

#define ASSERT_STATIC_OK(_mem, _context, _cond, _err)                                                         \
    do {                                                                                                      \
        struct kefir_ast_static_assertion *assert1 = kefir_ast_new_static_assertion((_mem), (_cond), (_err)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(assert1)));                  \
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert1)));                               \
    } while (0)

#define ASSERT_STATIC_NOK(_mem, _context, _cond, _err)                                                           \
    do {                                                                                                         \
        struct kefir_ast_static_assertion *assert1 = kefir_ast_new_static_assertion((_mem), (_cond), (_err));    \
        ASSERT(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(assert1)) == KEFIR_STATIC_ASSERT); \
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert1)));                                  \
    } while (0)

ASSERT_STATIC_OK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                 KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "ErrorA"));

ASSERT_STATIC_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
                  KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "ErrorB"));

ASSERT_STATIC_OK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                 KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error1"));

ASSERT_STATIC_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
                  KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error2"));

ASSERT_STATIC_OK(&kft_mem, context,
                 KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                     &kft_mem, KEFIR_AST_OPERATION_ADD, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                     KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)))),
                 KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error3"));

ASSERT_STATIC_NOK(
    &kft_mem, context,
    KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
                                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)),
                                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
    KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error4"));

ASSERT_STATIC_OK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")),
                 KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error5"));

ASSERT_STATIC_NOK(&kft_mem, context,
                  KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                      &kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                      KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")))),
                  KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error6"));

#undef ASSERT_STATIC_OK
#undef ASSERT_STATIC_NOK

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_expression_statements1, "AST node analysis - expression statements #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_expression_statement *stmt1 =
    kefir_ast_new_expression_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt1->expression->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);

struct kefir_ast_expression_statement *stmt2 = kefir_ast_new_expression_statement(
    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                  &kft_mem, KEFIR_AST_OPERATION_ADD, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, '\0')),
                  KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 6.00001)))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt2->expression->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);

struct kefir_ast_expression_statement *stmt3 = kefir_ast_new_expression_statement(&kft_mem, NULL);
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));
ASSERT(stmt3->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

struct kefir_ast_expression_statement *stmt4 = kefir_ast_new_expression_statement(
    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                  &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)))));
ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt4)));

struct kefir_ast_type_name *type_name1 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));

struct kefir_ast_expression_statement *stmt5 =
    kefir_ast_new_expression_statement(&kft_mem, KEFIR_AST_NODE_BASE(type_name1));
ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt5)));

KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt4));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt5));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_labeled_statements1, "AST node analysis - labeled statements #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_labeled_statement *stmt1 =
    kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label1",
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'A')))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt1->label != NULL);
ASSERT(strcmp(stmt1->label, "label1") == 0);
ASSERT(stmt1->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

struct kefir_ast_labeled_statement *stmt2 = kefir_ast_new_labeled_statement(
    &kft_mem, context->symbols, "label2",
    KEFIR_AST_NODE_BASE(kefir_ast_new_labeled_statement(
        &kft_mem, context->symbols, "label3",
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'B')))))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt2->base.properties.statement_props.label.string != NULL);
ASSERT(strcmp(stmt2->base.properties.statement_props.label.string, "label2") == 0);
ASSERT(stmt2->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt2->statement->properties.statement_props.label.string != NULL);
ASSERT(strcmp(stmt2->statement->properties.statement_props.label.string, "label3") == 0);

struct kefir_ast_labeled_statement *stmt3 = kefir_ast_new_labeled_statement(
    &kft_mem, context->symbols, "label4", KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 5.0048)));
ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));

KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_case_statements1, "AST node analysis - case statements #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "X",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 1004),
                                                  context->type_traits->underlying_enumeration_type, NULL));
ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "whatever", kefir_ast_type_signed_int(),
                                                   NULL, NULL));

struct kefir_ast_case_statement *stmt1 =
    kefir_ast_new_case_statement(&kft_mem, NULL,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                     &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, '\n')))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));
ASSERT(stmt1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt1->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

struct kefir_ast_case_statement *stmt2 =
    kefir_ast_new_case_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                     &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, '\n')))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));
ASSERT(stmt2->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt2->base.properties.statement_props.label.integer == 1);
ASSERT(stmt2->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

struct kefir_ast_case_statement *stmt3 =
    kefir_ast_new_case_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(&kft_mem, 0xffffe)),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                     &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'B')))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));
ASSERT(stmt3->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt3->base.properties.statement_props.label.integer == 0xffffe);
ASSERT(stmt3->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

struct kefir_ast_case_statement *stmt4 =
    kefir_ast_new_case_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(&kft_mem, -1)),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                                     &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, ' ')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                         &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'C')))))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt4)));
ASSERT(stmt4->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt4->base.properties.statement_props.label.integer == -1);
ASSERT(stmt4->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt4->statement->properties.statement_props.label.integer == ' ');

struct kefir_ast_case_statement *stmt5 =
    kefir_ast_new_case_statement(&kft_mem, NULL,
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_case_statement(
                                     &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'X')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                         &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'D')))))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt5)));
ASSERT(stmt5->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt5->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt5->statement->properties.statement_props.label.integer == 'X');

struct kefir_ast_case_statement *stmt6 =
    kefir_ast_new_case_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 0.0f)),
                                 KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                     &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'E')))));
ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt6)));

struct kefir_ast_case_statement *stmt7 = kefir_ast_new_case_statement(
    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")),
    KEFIR_AST_NODE_BASE(
        kefir_ast_new_expression_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'F')))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt7)));
ASSERT(stmt7->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);
ASSERT(stmt7->base.properties.statement_props.label.integer == 1004);
ASSERT(stmt7->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

struct kefir_ast_case_statement *stmt8 = kefir_ast_new_case_statement(
    &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "whatever")),
    KEFIR_AST_NODE_BASE(
        kefir_ast_new_expression_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'G')))));
ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt8)));

KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt4));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt5));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt6));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt7));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt8));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_labeled_statements2, "AST node analysis - labeled statements #2")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

const struct kefir_ast_scoped_identifier *scoped_id = NULL;
ASSERT(context->resolve_label_identifier(context, "label1", &scoped_id) == KEFIR_NOT_FOUND);
ASSERT(context->resolve_label_identifier(context, "label2", &scoped_id) == KEFIR_NOT_FOUND);
ASSERT(context->resolve_label_identifier(context, "label3", &scoped_id) == KEFIR_NOT_FOUND);
ASSERT(context->resolve_label_identifier(context, "label4", &scoped_id) == KEFIR_NOT_FOUND);

struct kefir_ast_labeled_statement *stmt1 =
    kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label1",
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt1)));

ASSERT_OK(context->resolve_label_identifier(context, "label1", &scoped_id));
ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
ASSERT(scoped_id->label.defined);

struct kefir_ast_labeled_statement *stmt2 =
    kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label1",
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt2)));

struct kefir_ast_labeled_statement *stmt3 =
    kefir_ast_new_labeled_statement(&kft_mem, context->symbols, "label2",
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                                        &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'c')))));
ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt3)));

ASSERT_OK(context->resolve_label_identifier(context, "label2", &scoped_id));
ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
ASSERT(scoped_id->label.defined);

struct kefir_ast_labeled_statement *stmt4;
struct kefir_ast_labeled_statement *stmt5;
struct kefir_ast_labeled_statement *stmt6;
do {
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &local_context));
    stmt4 = kefir_ast_new_labeled_statement(
        &kft_mem, context->symbols, "label3",
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'd')))));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt4)));

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &local_context));
        stmt5 = kefir_ast_new_labeled_statement(
            &kft_mem, context->symbols, "label4",
            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'e')))));
        ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt5)));

        stmt6 = kefir_ast_new_labeled_statement(
            &kft_mem, context->symbols, "label2",
            KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'f')))));
        ASSERT_NOK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(stmt6)));
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&local_context));
    } while (0);

    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&local_context));
} while (0);

ASSERT_OK(context->resolve_label_identifier(context, "label3", &scoped_id));
ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
ASSERT(scoped_id->label.defined);

ASSERT_OK(context->resolve_label_identifier(context, "label4", &scoped_id));
ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
ASSERT(scoped_id->label.defined);

KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt1));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt2));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt3));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt4));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt5));
KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(stmt6));
ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
