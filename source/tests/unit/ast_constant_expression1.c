#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/test/util.h"
#include "constant_expression.h"

DEFINE_CASE(ast_constant_expression_constant1, "AST constant expressions - constants #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_bool(&kft_mem, true), true);
ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_bool(&kft_mem, false), false);

for (kefir_char_t c = KEFIR_CHAR_MIN; c < KEFIR_CHAR_MAX; c++) {
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_char(&kft_mem, c), c);
}

for (kefir_int_t i = -1000; i < 1000; i++) {
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_int(&kft_mem, i), i);
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_long(&kft_mem, i), i);
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_long_long(&kft_mem, i), i);
}

for (kefir_uint_t i = 0; i < 10000; i++) {
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_uint(&kft_mem, i), i);
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_ulong(&kft_mem, i), i);
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_ulong_long(&kft_mem, i), i);
}

for (kefir_float32_t f = -100.00f; f < 100.00f; f += 0.01f) {
    ASSERT_FLOAT_CONST_EXPR(&kft_mem, context, kefir_ast_new_constant_float(&kft_mem, f), f);
}

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_identifier1, "AST constant expressions - identifiers #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "x",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 0),
                                                  type_traits->underlying_enumeration_type, NULL));
ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "y",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 1),
                                                  type_traits->underlying_enumeration_type, NULL));
ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "z",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 2),
                                                  type_traits->underlying_enumeration_type, NULL));

ASSERT_OK(kefir_ast_local_context_define_auto(
    &kft_mem, &local_context, "var1",
    kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_char(),
                             (struct kefir_ast_type_qualification){.constant = true}),
    NULL, NULL, NULL));

ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_identifier(&kft_mem, context->symbols, "x"), 0);
ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_identifier(&kft_mem, context->symbols, "y"), 1);
ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, kefir_ast_new_identifier(&kft_mem, context->symbols, "z"), 2);

ASSERT_CONST_EXPR_NOK(&kft_mem, context, kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"));

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_string_literal1, "AST constant expressions - string literals")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

ASSERT_LITERAL_CONST_EXPR(&kft_mem, context, KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, ""), "");
ASSERT_LITERAL_CONST_EXPR(&kft_mem, context, KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, world!"), "Hello, world!");
ASSERT_LITERAL_CONST_EXPR(&kft_mem, context, KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, cruel world!"),
                          "Hello, cruel world!");
ASSERT_LITERAL_CONST_EXPR(&kft_mem, context, KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "\n\naaaAAA\tAbc\n   \tCBA\n\t\t"),
                          "\n\naaaAAA\tAbc\n   \tCBA\n\t\t");

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_generic_selection1, "AST constant expressions - generic selections")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_type_name *type_name1 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                     kefir_ast_type_specifier_char(&kft_mem)));

struct kefir_ast_type_name *type_name2 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));

struct kefir_ast_type_name *type_name3 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));

struct kefir_ast_generic_selection *selection1 =
    kefir_ast_new_generic_selection(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection1,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection1,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection1,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, selection1, 1);

struct kefir_ast_generic_selection *selection2 =
    kefir_ast_new_generic_selection(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 0)));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection2,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection2,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection2,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, selection2, 2);

struct kefir_ast_generic_selection *selection3 =
    kefir_ast_new_generic_selection(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'A')));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection3,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection3,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection3,
    (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, selection3, 0);

struct kefir_ast_generic_selection *selection4 =
    kefir_ast_new_generic_selection(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 2.71)));
ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, type_name1,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, type_name2,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, NULL,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100))));
ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, type_name3,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

ASSERT_INTEGER_CONST_EXPR(&kft_mem, context, selection4, 100);

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_binary_operations1, "AST constant expressions - binary operations #1")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

for (kefir_int64_t i = -10; i < 10; i++) {
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_ADD,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i * 2))),
        i * 3);
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i * 2))),
        -i);
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -3))),
        -i * 3);
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_DIVIDE,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i * i * 10)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i != 0 ? i * 2 : 1))),
        i != 0 ? 5 * i : 0);
    ASSERT_INTEGER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MODULO,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i + 17)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 13))),
        (i + 17) % 13);

    if (i >= 0) {
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SHIFT_LEFT,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 3))),
            i << 3);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SHIFT_RIGHT,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, ~0)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            ~0 >> i);
    }

    for (kefir_int64_t j = -10; j < 10; j++) {
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i < j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i <= j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i > j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i >= j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i == j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_NOT_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i != j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_BITWISE_AND,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i & j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_BITWISE_OR,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i | j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_BITWISE_XOR,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i ^ j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i && j);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_OR,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
            i || j);
    }
}

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_binary_operations2, "AST constant expressions - binary operations #2")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

for (kefir_float64_t f = -5.0; f < 5.0; f += 0.1) {
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_ADD,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f * 2))),
        f + f * 2);
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f * 2))),
        f - f * 2);
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, -3))),
        f * -3.0f);
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_DIVIDE,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f * f * 10)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f * 2))),
        (f * f * 10) / (f * 2));

    for (kefir_float64_t f2 = -5.0; f2 < 5.0; f2 += 0.1) {
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f < f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f <= f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f >= f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f > f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f == f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_NOT_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f != f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f && f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_OR,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
            f || f2);
    }
}

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_binary_operations3, "AST constant expressions - binary operations #3")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

for (kefir_float64_t f = -5.0; f < 5.0; f += 0.1) {
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_ADD,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, (int) f * 2))),
        f + (int) f * 2);
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, (int) f * 2))),
        f - (int) f * 2);
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -3))),
        f * (int) -3.0f);
    ASSERT_FLOAT_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_DIVIDE,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f * f * 10)),
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, (int) (f * 2)))),
        (f * f * 10) / (int) (f * 2));

    for (kefir_int64_t f2 = -5; f2 < 5; f2++) {
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f < f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f <= f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f >= f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f > f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f == f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_NOT_EQUAL,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f != f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f && f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_OR,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
            f || f2);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            f2 && f);
        ASSERT_INTEGER_CONST_EXPR(
            &kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_OR,
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2)),
                                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            f2 || f);
    }
}

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_binary_operations4, "AST constant expressions - binary operations #4")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_struct_type *struct_type1 = NULL;
const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type1);
ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "array",
                                      kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                           kefir_ast_constant_expression_integer(&kft_mem, 64), NULL),
                                      NULL));

ASSERT_OK(kefir_ast_global_context_define_static(
    &kft_mem, &global_context, "x",
    kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                             (struct kefir_ast_type_qualification){.constant = true}),
    NULL, NULL, NULL));

ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "y",
                                                 kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
                                                                          (struct kefir_ast_type_qualification){
                                                                              .constant = true}),
                                                 NULL, NULL, NULL));

for (kefir_int_t i = -100; i < 100; i++) {
    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
        "x", i * 4);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADD, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x"))))),
        "x", i * 4);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
        "x", -i * 4);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
        "y", i * 64);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADD, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y"))))),
        "y", i * 64);

    ASSERT_IDENTIFIER_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
        "y", -i * 64);
}

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
