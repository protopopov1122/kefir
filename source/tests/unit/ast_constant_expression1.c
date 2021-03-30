#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/test/util.h"

#define ASSERT_INTEGER_CONST_EXPR(_mem, _context, _node, _value) \
    do { \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base)); \
        struct kefir_ast_constant_expression_value value; \
        ASSERT_OK(kefir_ast_constant_expression_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER); \
        ASSERT(value.integer == (_value)); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base)); \
    } while (0)

#define ASSERT_FLOAT_CONST_EXPR(_mem, _context, _node, _value) \
    do { \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base)); \
        struct kefir_ast_constant_expression_value value; \
        ASSERT_OK(kefir_ast_constant_expression_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT); \
        ASSERT(DOUBLE_EQUALS(value.floating_point, (_value), DOUBLE_EPSILON)); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base)); \
    } while (0)

#define ASSERT_STRING_CONST_EXPR(_mem, _context, _node, _value) \
    do { \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base)); \
        struct kefir_ast_constant_expression_value value; \
        ASSERT_OK(kefir_ast_constant_expression_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS); \
        ASSERT(value.pointer->klass->type == KEFIR_AST_STRING_LITERAL); \
        ASSERT(strcmp(((struct kefir_ast_string_literal *) value.pointer->self)->literal, (_value)) == 0); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base)); \
    } while (0)

#define ASSERT_CONST_EXPR_NOK(_mem, _context, _node) \
    do { \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base)); \
        struct kefir_ast_constant_expression_value value; \
        ASSERT_NOK(kefir_ast_constant_expression_evaluate((_mem), (_context), base, &value)); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), base)); \
    } while (0)

DEFINE_CASE(ast_constant_expression_constant1, "AST constant expressions - constants #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_constant_bool(&kft_mem, true),
        true);
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_constant_bool(&kft_mem, false),
        false);

    for (kefir_char_t c = KEFIR_CHAR_MIN; c < KEFIR_CHAR_MAX; c++) {
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_char(&kft_mem, c),
            c);
    }

    for (kefir_int_t i = -1000; i < 1000; i++) {
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_int(&kft_mem, i),
            i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_long(&kft_mem, i),
            i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_long_long(&kft_mem, i),
            i);
    }

    for (kefir_uint_t i = 0; i < 10000; i++) {
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_uint(&kft_mem, i),
            i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_ulong(&kft_mem, i),
            i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_ulong_long(&kft_mem, i),
            i);
    }

    for (kefir_float32_t f = -100.00f; f < 100.00f; f += 0.01f) {
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_constant_float(&kft_mem, f),
            f);
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_identifier1, "AST constant expressions - identifiers #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context,
        "x", kefir_ast_constant_expression_integer(&kft_mem, 0), type_traits->underlying_enumeration_type));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context,
        "y", kefir_ast_constant_expression_integer(&kft_mem, 1), type_traits->underlying_enumeration_type));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context,
        "z", kefir_ast_constant_expression_integer(&kft_mem, 2), type_traits->underlying_enumeration_type));
    
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context,
        "var1", kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_char(),
            (struct kefir_ast_type_qualification){
                .constant = true
            }), NULL, NULL));

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "x"),
        0);
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "y"),
        1);
    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "z"),
        2);

    ASSERT_CONST_EXPR_NOK(&kft_mem, context,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_string_literal1, "AST constant expressions - string literals")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_STRING_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_string_literal(&kft_mem, context->symbols, ""),
        "") ;
    ASSERT_STRING_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_string_literal(&kft_mem, context->symbols, "Hello, world!"),
        "Hello, world!") ;
    ASSERT_STRING_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_string_literal(&kft_mem, context->symbols, "Hello, cruel world!"),
        "Hello, cruel world!") ;
    ASSERT_STRING_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_string_literal(&kft_mem, context->symbols, "\n\naaaAAA\tAbc\n   \tCBA\n\t\t"),
        "\n\naaaAAA\tAbc\n   \tCBA\n\t\t") ;

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_generic_selection1, "AST constant expressions - generic selections")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_generic_selection *selection1 = kefir_ast_new_generic_selection(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, context->type_traits,
        kefir_ast_type_unsigned_char(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, context->type_traits,
        kefir_ast_type_signed_int(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, context->type_traits,
        kefir_ast_type_float(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        selection1, 1);

    struct kefir_ast_generic_selection *selection2 = kefir_ast_new_generic_selection(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 0)));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection2, context->type_traits,
        kefir_ast_type_unsigned_char(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection2, context->type_traits,
        kefir_ast_type_signed_int(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection2, context->type_traits,
        kefir_ast_type_float(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        selection2, 2);

    struct kefir_ast_generic_selection *selection3 = kefir_ast_new_generic_selection(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'A')));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection3, context->type_traits,
        kefir_ast_type_unsigned_char(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection3, context->type_traits,
        kefir_ast_type_signed_int(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection3, context->type_traits,
        kefir_ast_type_float(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        selection3, 0);

    struct kefir_ast_generic_selection *selection4 = kefir_ast_new_generic_selection(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 2.71)));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, context->type_traits,
        kefir_ast_type_unsigned_char(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, context->type_traits,
        kefir_ast_type_signed_int(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, context->type_traits,
        NULL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100))));
    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection4, context->type_traits,
        kefir_ast_type_float(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        selection4, 100);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations1, "AST constant expressions - unary operations #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    for (kefir_int64_t i = -100; i < 100; i++) {
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_PLUS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            +i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_NEGATE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            -i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_INVERT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            ~i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            !i);
    }

    for (kefir_float64_t f = -100.0f; f < 100.0f; f += 0.01f) {
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_PLUS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            +f);
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_NEGATE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            -f);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
            !f);
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_binary_operations1, "AST constant expressions - binary operations #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    for (kefir_int64_t i = -10; i < 10; i++) {
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i*2))),
            i*3);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i*2))),
            -i);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -3))),
            -i*3);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_DIVIDE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i*i*10)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i != 0 ? i*2 : 1))),
            i != 0 ? 5*i : 0);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MODULO,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i + 17)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 13))),
            (i + 17) % 13);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SHIFT_LEFT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 3))),
            i << 3);
        ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SHIFT_RIGHT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, ~0)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            ~0 >> i);

        for (kefir_int64_t j = -10; j < 10; j++) {
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i < j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i <= j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i > j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i >= j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i == j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_NOT_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i != j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_BITWISE_AND,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i & j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_BITWISE_OR,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i | j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_BITWISE_XOR,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i ^ j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, j))),
                i && j);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    for (kefir_float64_t f = -5.0; f < 5.0; f += 0.1) {
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f*2))),
            f + f*2);
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f*2))),
            f - f*2);
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, -3))),
            f * -3.0f);
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_DIVIDE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f*f*10)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f*2))),
            (f*f*10)/(f*2));

        for (kefir_float64_t f2 = -5.0; f2 < 5.0; f2 += 0.1) {
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
                f < f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
                f <= f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
                f >= f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
                f > f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
                f == f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_NOT_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
                f != f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f2))),
                f && f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    for (kefir_float64_t f = -5.0; f < 5.0; f += 0.1) {
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, (int) f*2))),
            f + (int) f*2);
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, (int) f*2))),
            f - (int) f*2);
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -3))),
            f * (int) -3.0f);
        ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_DIVIDE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f*f*10)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, (int) (f*2)))),
            (f*f*10) / (int) (f*2));

        for (kefir_int64_t f2 = -5; f2 < 5; f2++) {
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f < f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LESS_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f <= f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f >= f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_GREATER,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f > f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f == f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_NOT_EQUAL,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f != f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f && f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_OR,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2))),
                f || f2);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_AND,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
                f2 && f);
            ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_OR,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, f2)),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
                f2 || f);
        }
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_conditional_operator1, "AST constant expressions - conditional operators #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 3))),
        3);

    ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 25))),
        3.14);

    ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 2.71))),
        2.71);

    ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 100.5)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, -1))),
        100.5);

    ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, -27.5)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 19.01))),
        19.01);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -2))),
        -1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_conditional_operator(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 14))),
        14);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations2, "AST constant expressions - unary operations #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_bool()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_char()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_char()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_char()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_short()))),
        2);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_short()))),
        2);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_int()))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_int()))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_long_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_long_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_float()))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_double()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void())))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                    kefir_ast_constant_expression_integer(&kft_mem, 8), NULL)))),
        8);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations3, "AST constant expressions - unary operations #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -100))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 200))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, -100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(&kft_mem, 100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(&kft_mem, -100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(&kft_mem, 100500))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71828f))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14159))),
        8);

    struct kefir_ast_compound_literal *compound1 = kefir_ast_new_compound_literal(&kft_mem,
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound1->initializer->list,
        kefir_ast_new_index_desginator(&kft_mem, 9, NULL),
        kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H')))));

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_SIZEOF,
            KEFIR_AST_NODE_BASE(compound1)),
        40);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations4, "AST constant expressions - unary operations #4")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_bool()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_char()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_char()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_char()))),
        1);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_short()))),
        2);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_short()))),
        2);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_int()))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_int()))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_unsigned_long_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_signed_long_long()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_float()))),
        4);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_double()))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_short())))),
        8);

    ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ALIGNOF,
            KEFIR_AST_NODE_BASE(kefir_ast_new_type_name(&kft_mem,
                kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                    kefir_ast_constant_expression_integer(&kft_mem, 5), NULL)))),
        4);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE