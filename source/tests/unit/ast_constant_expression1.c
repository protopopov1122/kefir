#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"

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
        ASSERT(value.referred_value->klass->type == KEFIR_AST_STRING_LITERAL); \
        ASSERT(strcmp(((struct kefir_ast_string_literal *) value.referred_value->self)->literal, (_value)) == 0); \
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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