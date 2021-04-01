#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/test/util.h"
#include "constant_expression.h"

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

DEFINE_CASE(ast_constant_expression_cast_operator2, "AST constant expressions - cast operator #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    for (kefir_size_t i = 0; i < 100; i++) {
        ASSERT_INTPTR_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                kefir_ast_type_void()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);

        ASSERT_INTPTR_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                kefir_ast_type_signed_long()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);

        ASSERT_INTPTR_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                kefir_ast_type_double()),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);

        ASSERT_INTPTR_CONST_EXPR(&kft_mem, context,
            kefir_ast_new_cast_operator(&kft_mem, kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int())),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))),
            i, 0);
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