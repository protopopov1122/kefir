#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

struct kefir_ast_constant *make_constant(struct kefir_mem *, const struct kefir_ast_type *);

#define ASSERT_COMPOUND_ASSIGNMENT(_mem, _context, _oper, _target, _value, _type) \
    do { \
        struct kefir_ast_assignment_operator *oper = kefir_ast_new_compound_assignment((_mem), (_oper), \
            (_target), (_value)); \
        ASSERT(oper != NULL); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper))); \
        ASSERT(oper->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, (_type))); \
        ASSERT(!oper->base.properties.expression_props.lvalue); \
        ASSERT(!oper->base.properties.expression_props.bitfield); \
        ASSERT(!oper->base.properties.expression_props.addressable); \
        ASSERT(!oper->base.properties.expression_props.constant_expression); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper))); \
    } while (0)

#define ASSERT_COMPOUND_ASSIGNMENT_NOK(_mem, _context, _oper, _target, _value) \
    do { \
        struct kefir_ast_assignment_operator *oper = kefir_ast_new_compound_assignment((_mem), (_oper), \
            (_target), (_value)); \
        ASSERT(oper != NULL); \
        ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper))); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper))); \
    } while (0)

DEFINE_CASE(ast_node_analysis_compound_assignment_operator1, "AST node analysis - compound assignment operator #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_bool(),
        kefir_ast_type_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double()
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "x", kefir_ast_type_signed_int(), NULL));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "y", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()), NULL));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "z", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), NULL));

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, KEFIR_AST_ASSIGNMENT_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem,
                KEFIR_AST_OPERATION_INDIRECTION,
                KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem, 
                    kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))))),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            TYPES[i]);

        ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, KEFIR_AST_ASSIGNMENT_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            kefir_ast_type_signed_int());

        if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(TYPES[i])) {
            ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, KEFIR_AST_ASSIGNMENT_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
                kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()));
        } else {
            ASSERT_COMPOUND_ASSIGNMENT_NOK(&kft_mem, context, KEFIR_AST_ASSIGNMENT_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));
        }

        ASSERT_COMPOUND_ASSIGNMENT_NOK(&kft_mem, context, KEFIR_AST_ASSIGNMENT_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));

        ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, KEFIR_AST_ASSIGNMENT_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem,
                KEFIR_AST_OPERATION_INDIRECTION,
                KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem, 
                    kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))))),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            TYPES[i]);

        ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, KEFIR_AST_ASSIGNMENT_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            kefir_ast_type_signed_int());

        if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(TYPES[i])) {
            ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, KEFIR_AST_ASSIGNMENT_SUBTRACT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
                kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()));
        } else {
            ASSERT_COMPOUND_ASSIGNMENT_NOK(&kft_mem, context, KEFIR_AST_ASSIGNMENT_SUBTRACT,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));
        }

        ASSERT_COMPOUND_ASSIGNMENT_NOK(&kft_mem, context, KEFIR_AST_ASSIGNMENT_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "z")),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_compound_assignment_operator2, "AST node analysis - compound assignment operator #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "x", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()), NULL));

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_bool(),
        kefir_ast_type_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double()
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    kefir_ast_assignment_operation_t OPERATORS[] = {
        KEFIR_AST_ASSIGNMENT_MODULO,
        KEFIR_AST_ASSIGNMENT_SHIFT_LEFT,
        KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT,
        KEFIR_AST_ASSIGNMENT_BITWISE_AND,
        KEFIR_AST_ASSIGNMENT_BITWISE_OR,
        KEFIR_AST_ASSIGNMENT_BITWISE_XOR
    };
    const kefir_size_t OPERATORS_LENGTH = sizeof(OPERATORS) / sizeof(OPERATORS[0]);

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t k = 0; k < OPERATORS_LENGTH; k++) {
            for (kefir_size_t j = 0; j < TYPES_LEN; j++) {
                if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(TYPES[i]) && KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(TYPES[j])) {
                    ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, OPERATORS[k],
                        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem,
                            KEFIR_AST_OPERATION_INDIRECTION,
                            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem, 
                                kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]),
                                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))))),
                        KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[j])),
                        TYPES[i]);
                } else {
                    ASSERT_COMPOUND_ASSIGNMENT_NOK(&kft_mem, context, OPERATORS[k],
                        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem,
                            KEFIR_AST_OPERATION_INDIRECTION,
                            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem, 
                                kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]),
                                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))))),
                        KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[j])));
                }
            }
            ASSERT_COMPOUND_ASSIGNMENT_NOK(&kft_mem, context, OPERATORS[k],
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));
        }
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_compound_assignment_operator3, "AST node analysis - compound assignment operator #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "x", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()), NULL));

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_bool(),
        kefir_ast_type_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double()
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    kefir_ast_assignment_operation_t OPERATORS[] = {
        KEFIR_AST_ASSIGNMENT_MULTIPLY,
        KEFIR_AST_ASSIGNMENT_DIVIDE
    };
    const kefir_size_t OPERATORS_LENGTH = sizeof(OPERATORS) / sizeof(OPERATORS[0]);

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t k = 0; k < OPERATORS_LENGTH; k++) {
            for (kefir_size_t j = 0; j < TYPES_LEN; j++) {
                ASSERT_COMPOUND_ASSIGNMENT(&kft_mem, context, OPERATORS[k],
                    KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem,
                        KEFIR_AST_OPERATION_INDIRECTION,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem, 
                            kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]),
                            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))))),
                    KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[j])),
                    TYPES[i]);
            }
            ASSERT_COMPOUND_ASSIGNMENT_NOK(&kft_mem, context, OPERATORS[k],
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));
        }
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_comma_operator, "AST node analysis - comma operator")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "x", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context,
        "y", kefir_ast_constant_expression_integer(&kft_mem, 100), type_traits->underlying_enumeration_type));

    struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(&kft_mem);
    ASSERT(comma != NULL);
    ASSERT_OK(kefir_ast_comma_append(&kft_mem, comma,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))));
    ASSERT_OK(kefir_ast_comma_append(&kft_mem, comma,
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 0))))));
    ASSERT_OK(kefir_ast_comma_append(&kft_mem, comma,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x"))));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(comma)));
    ASSERT(comma->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(!comma->base.properties.expression_props.lvalue);
    ASSERT(!comma->base.properties.expression_props.constant_expression);
    ASSERT(!comma->base.properties.expression_props.bitfield);
    ASSERT(!comma->base.properties.expression_props.addressable);
    ASSERT(KEFIR_AST_TYPE_SAME(comma->base.properties.type, kefir_ast_type_float()));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(comma)));

    comma = kefir_ast_new_comma_operator(&kft_mem);
    ASSERT(comma != NULL);
    ASSERT_OK(kefir_ast_comma_append(&kft_mem, comma,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y"))));
    ASSERT_OK(kefir_ast_comma_append(&kft_mem, comma,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))));
    ASSERT_OK(kefir_ast_comma_append(&kft_mem, comma,
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 0))))));

    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(comma)));
    ASSERT(comma->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(!comma->base.properties.expression_props.lvalue);
    ASSERT(!comma->base.properties.expression_props.constant_expression);
    ASSERT(!comma->base.properties.expression_props.bitfield);
    ASSERT(!comma->base.properties.expression_props.addressable);
    ASSERT(KEFIR_AST_TYPE_SAME(comma->base.properties.type,
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char())));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(comma)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE