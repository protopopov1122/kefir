#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

#define ASSERT_FUNCTION_CALL(_mem, _context, _id, _type, _args) \
    do { \
        struct kefir_ast_function_call *call1 = kefir_ast_new_function_call( \
            (_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_id)))); \
        _args \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(call1))); \
        ASSERT(call1->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(call1->base.properties.type, (_type))); \
        ASSERT(!call1->base.properties.expression_props.constant_expression); \
        ASSERT(!call1->base.properties.expression_props.lvalue); \
        ASSERT(!call1->base.properties.expression_props.addressable); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(call1)); \
    } while (0)

#define ASSERT_FUNCTION_CALL_NOK(_mem, _context, _id, _args) \
    do { \
        struct kefir_ast_function_call *call1 = kefir_ast_new_function_call( \
            (_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_id)))); \
        _args \
        ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(call1))); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(call1)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_function_calls1, "AST node analysis - function calls #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_float(), "func1", &function1);

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_void(), "func2", &function2);

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type1));
    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
        type2));

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_void(), {
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_void(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_void(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_function_calls2, "AST node analysis - function calls #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_signed_short(), "func1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        NULL, kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        NULL, kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        NULL, kefir_ast_type_char(), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_unsigned_short(), "func2", &function2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2,
        NULL, kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2,
        NULL, kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2,
        NULL, kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_type_function_ellipsis(function2, true));

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type1));
    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type2));

    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
    });
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0f))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f))));
    });
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });


    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func2", {
    });
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func2", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_unsigned_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_unsigned_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0f))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_unsigned_short(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_function_calls3, "AST node analysis - function calls #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_signed_int(), "func1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        "a", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        "b", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        "c", NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type1));

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 0.0f))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f))));
    });
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 1.0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a'))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_function_calls4, "AST node analysis - function calls #4")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_float(), "func1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        NULL, kefir_ast_type_void(), NULL));

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        type1));

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_float(), {
    });
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
    });
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func1", {
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0))));
        ASSERT_OK(kefir_ast_function_call_append(&kft_mem, call1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#define ASSERT_COMPOUND_LITERAL(_mem, _context, _type, _init, _result_type, _constant) \
    do { \
        struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal((_mem), (_type));\
        ASSERT(compound != NULL); \
        _init \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(compound))); \
        ASSERT(compound->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(compound->base.properties.type, (_result_type))); \
        ASSERT(compound->base.properties.expression_props.constant_expression == (_constant)); \
        ASSERT(compound->base.properties.expression_props.lvalue); \
        ASSERT(compound->base.properties.expression_props.addressable); \
        ASSERT(!compound->base.properties.expression_props.bitfield); \
        ASSERT(!compound->base.properties.expression_props.string_literal.content); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(compound))); \
    } while (0)

#define ASSERT_COMPOUND_LITERAL_NOK(_mem, _context, _type, _init) \
    do { \
        struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal((_mem), (_type));\
        ASSERT(compound != NULL); \
        _init \
        ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(compound))); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(compound))); \
    } while (0)

DEFINE_CASE(ast_node_analysis_compound_literal1, "AST node analysis - compound literals #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "x", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context,
        "y", kefir_ast_constant_expression_integer(&kft_mem, 100), type_traits->underlying_enumeration_type));

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

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, TYPES[i], {
        });

        ASSERT_COMPOUND_LITERAL(&kft_mem, context, TYPES[i], {
            ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
                NULL, kefir_ast_new_expression_initializer(&kft_mem,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)))));
        }, TYPES[i], true);

        ASSERT_COMPOUND_LITERAL(&kft_mem, context, TYPES[i], {
            ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
                NULL, kefir_ast_new_expression_initializer(&kft_mem,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))));
        }, TYPES[i], false);

        ASSERT_COMPOUND_LITERAL(&kft_mem, context, TYPES[i], {
            ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
                NULL, kefir_ast_new_expression_initializer(&kft_mem,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")))));
        }, TYPES[i], true);

        if (i != 0) {
            ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, TYPES[i], {
                ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
                    NULL, kefir_ast_new_expression_initializer(&kft_mem,
                    KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, world!")))));
            });
        } else {
            ASSERT_COMPOUND_LITERAL(&kft_mem, context, TYPES[i], {
                ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
                    NULL, kefir_ast_new_expression_initializer(&kft_mem,
                    KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, world!")))));
            }, TYPES[i], true);
        }
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_compound_literal2, "AST node analysis - compound literals #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "x", kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
            kefir_ast_constant_expression_integer(&kft_mem, 10), NULL), NULL));

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle,
        kefir_ast_type_char(), NULL);
    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(&kft_mem, context->type_bundle,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_char(),
            (struct kefir_ast_type_qualification){
                .constant = true,
                .restricted = false,
                .volatile_type = false
            }));
    
    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type1, {
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
        kefir_ast_constant_expression_integer(&kft_mem, 0), NULL), true);
    
    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type1, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, world!")))));
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
        kefir_ast_constant_expression_integer(&kft_mem, 14), NULL), true);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type1, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'e')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'o')))));
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
        kefir_ast_constant_expression_integer(&kft_mem, 5), NULL), true);
    
    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type2, {
    });
    
    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, world!")))));
    }, type2, true);
    
    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'e')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'l')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'o')))));
    }, type2, true);
    
    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")))));
    }, type2, false);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_compound_literal3, "AST node analysis - compound literals #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "field1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "field2", kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "field3", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), NULL));

    struct kefir_ast_struct_type *structure2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "x", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "y", kefir_ast_type_array(&kft_mem, context->type_bundle, type1,
            kefir_ast_constant_expression_integer(&kft_mem, 2), NULL), NULL));

    const struct kefir_ast_type *type3 = kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle,
        type2, NULL);

    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &local_context,
        "var1", type1, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context,
        "var2", type2, NULL));
    
    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
    }, type2, true);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
    }, type2, true);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
    }, type2, true);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
    }, type2, false);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
    }, type2, false);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    });

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
    }, type2, false);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem,
                KEFIR_AST_OPERATION_ADDRESS,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))))));
    }, type2, false);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type2, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
    }, type2, true);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type3, {
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
        kefir_ast_constant_expression_integer(&kft_mem, 0), NULL), true);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type3, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
    });

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type3, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
        kefir_ast_constant_expression_integer(&kft_mem, 1), NULL), false);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type3, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
        kefir_ast_constant_expression_integer(&kft_mem, 4), NULL), false);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type3, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    });

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type3, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
        kefir_ast_constant_expression_integer(&kft_mem, 2), NULL), true);

    ASSERT_COMPOUND_LITERAL(&kft_mem, context, type3, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var1")))));
    }, kefir_ast_type_array(&kft_mem, context->type_bundle, type2,
        kefir_ast_constant_expression_integer(&kft_mem, 2), NULL), false);

    ASSERT_COMPOUND_LITERAL_NOK(&kft_mem, context, type3, {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1001)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'b')))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));

        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 2.71f)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "var2")))));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_COMPOUND_LITERAL
#undef ASSERT_COMPOUND_LITERAL_NOK

#define ASSERT_COMPOUND_LITERAL_TEMP(_mem, _context, _type, _init, _id, _member) \
    do { \
        struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal((_mem), (_type));\
        ASSERT(compound != NULL); \
        _init \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(compound))); \
        ASSERT(compound->base.properties.expression_props.temporary.identifier == (_id)); \
        ASSERT(compound->base.properties.expression_props.temporary.field == (_member)); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(compound))); \
    } while (0)

DEFINE_CASE(ast_node_analysis_compound_literal4, "AST node analysis - compound literals temporaries")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT(context->resolve_ordinary_identifier(context,
        KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, &scoped_id) == KEFIR_NOT_FOUND);

    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context, kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));  
    }, 0, 0);
    ASSERT_OK(context->resolve_ordinary_identifier(context,
        KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, &scoped_id));

    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context, kefir_ast_type_signed_char(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));  
    }, 0, 1);
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context, kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));  
    }, 0, 2);

    ASSERT_OK(kefir_ast_temporaries_next_block(context->temporaries));

    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context, kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));  
    }, 1, 0);
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context, kefir_ast_type_signed_char(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));  
    }, 1, 1);
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context, kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));  
    }, 1, 2);

    ASSERT_OK(kefir_ast_temporaries_next_block(context->temporaries));
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context,
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_bool(), NULL), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)))));
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)))));  
    }, 2, 0);
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, context, kefir_ast_type_signed_long(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 10000)))));  
    }, 2, 1);

    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, &global_context.context, kefir_ast_type_signed_long(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 10000)))));  
    }, 0, 0);
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, &global_context.context, kefir_ast_type_signed_int(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)))));  
    }, 0, 1);
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, &global_context.context, kefir_ast_type_signed_char(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));  
    }, 0, 2);
    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, &global_context.context, kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));  
    }, 0, 3);
    ASSERT_OK(kefir_ast_temporaries_next_block(&global_context.temporaries));

    ASSERT_COMPOUND_LITERAL_TEMP(&kft_mem, &global_context.context, kefir_ast_type_float(), {
        ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list,
            NULL, kefir_ast_new_expression_initializer(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 5.1f)))));  
    }, 1, 0);

    ASSERT_OK(context->resolve_ordinary_identifier(context,
        KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER, &scoped_id));
        
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
