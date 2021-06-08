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

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context,
        "X", kefir_ast_constant_expression_integer(&kft_mem, 1),
        context->type_traits->underlying_enumeration_type, NULL));

#define ASSERT_STATIC_OK(_mem, _context, _cond, _err) \
    do { \
        struct kefir_ast_static_assertion *assert1 = kefir_ast_new_static_assertion((_mem), \
            (_cond), (_err)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(assert1))); \
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert1))); \
    } while (0)

#define ASSERT_STATIC_NOK(_mem, _context, _cond, _err) \
    do { \
        struct kefir_ast_static_assertion *assert1 = kefir_ast_new_static_assertion((_mem), \
            (_cond), (_err)); \
        ASSERT(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(assert1)) == KEFIR_STATIC_ASSERT); \
        ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert1))); \
    } while (0)

    ASSERT_STATIC_OK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "ErrorA"));

    ASSERT_STATIC_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "ErrorB"));

    ASSERT_STATIC_OK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error1"));

    ASSERT_STATIC_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error2"));

    ASSERT_STATIC_OK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)))),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error3"));

    ASSERT_STATIC_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_MULTIPLY,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1000)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error4"));

    ASSERT_STATIC_OK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error5"));

    ASSERT_STATIC_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")))),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Error6"));

#undef ASSERT_STATIC_OK
#undef ASSERT_STATIC_NOK

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
