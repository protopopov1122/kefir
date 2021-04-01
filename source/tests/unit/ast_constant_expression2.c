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

#define ASSERT_IDENTIFIER_CONST_EXPR(_mem, _context, _node, _value, _offset) \
    do { \
        struct kefir_ast_node_base *base = KEFIR_AST_NODE_BASE((_node)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), base)); \
        struct kefir_ast_constant_expression_value value; \
        ASSERT_OK(kefir_ast_constant_expression_evaluate((_mem), (_context), base, &value)); \
        ASSERT(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS); \
        ASSERT(value.pointer.type == KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER); \
        ASSERT(strcmp(value.pointer.base.literal, (_value)) == 0); \
        ASSERT(value.pointer.offset == (_offset)); \
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

DEFINE_CASE(ast_constant_expression_cast_operator1, "AST constant expressions - cast operator #1")
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
        kefir_ast_type_unsigned_short(),
        kefir_ast_type_signed_short(),
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

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        const struct kefir_ast_type *type = TYPES[i];
        for (kefir_int_t j = -10; j < 10; j++) {
            if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type)) { 
                ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, j))),
                    j);
            } else {
                ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, j))),
                    (double) j);
            }
        }
    }

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        const struct kefir_ast_type *type = TYPES[i];
        for (kefir_float64_t f = -10.0f; f < 10.0f; f += 0.1f) {
            if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type)) { 
                ASSERT_INTEGER_CONST_EXPR(&kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
                    (kefir_int64_t) f);
            } else {
                ASSERT_FLOAT_CONST_EXPR(&kft_mem, context,
                    kefir_ast_new_cast_operator(&kft_mem, type,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, f))),
                    f);
            }
        }
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_constant_expression_unary_operations5, "AST constant expressions - unary operations #5")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &struct1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1,
        "fieldX", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1,
        "fieldY", kefir_ast_type_signed_int(), kefir_ast_alignment_const_expression(&kft_mem,
            kefir_ast_constant_expression_integer(&kft_mem, 8))));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1,
        "fieldZ", kefir_ast_type_array(&kft_mem, context->type_bundle,
            kefir_ast_type_signed_long(), kefir_ast_constant_expression_integer(&kft_mem, 8), NULL), NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context,
        "var1", kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){
                .constant = true
            }), NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context,
        "var2", kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
            (struct kefir_ast_type_qualification){
                .constant = true
            }), NULL));

    ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"))),
        "var1", 0);

    ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"))),
        "var2", 0);

    ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(&kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")),
                "fieldX"))),
        "var2", 0);

    ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(&kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")),
                "fieldY"))),
        "var2", 8);

    ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
        kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS,
            KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(&kft_mem, context->symbols,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")),
                "fieldZ"))),
        "var2", 16);

    for (kefir_size_t i = 0; i < 8; i++) {
        ASSERT_IDENTIFIER_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(&kft_mem,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_struct_member(&kft_mem, context->symbols,
                        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")),
                        "fieldZ")),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))))),
            "var2", 16 + i * 8);
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE