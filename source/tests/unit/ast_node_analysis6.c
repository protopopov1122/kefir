#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"

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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
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