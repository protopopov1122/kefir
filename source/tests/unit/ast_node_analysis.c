#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/analyzer.h"

#define ASSERT_CONSTANT(_mem, _context, _cnst, _const_type) \
    do { \
        struct kefir_ast_constant *const1 = (_cnst); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(const1))); \
        ASSERT(const1->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(const1->base.properties.type, (_const_type))); \
        ASSERT(const1->base.properties.expression_props.constant_expression); \
        ASSERT(!const1->base.properties.expression_props.lvalue); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(const1)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_constants, "AST node analysis - constant types")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_bool(&kft_mem, false), kefir_ast_type_bool());
    ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_bool(&kft_mem, true), kefir_ast_type_bool());

    for (kefir_char_t i = KEFIR_CHAR_MIN; i < KEFIR_CHAR_MAX; i++) {
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_char(&kft_mem, i), kefir_ast_type_char());
    }

    for (kefir_int_t i = -1000; i < 1000; i++) {
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_int(&kft_mem, i), kefir_ast_type_signed_int());
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_long(&kft_mem, i), kefir_ast_type_signed_long());
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_long_long(&kft_mem, i), kefir_ast_type_signed_long_long());
    }

    for (kefir_uint_t i = 0; i < 10000; i++) {
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_uint(&kft_mem, i), kefir_ast_type_unsigned_int());
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_ulong(&kft_mem, i), kefir_ast_type_unsigned_long());
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_ulong_long(&kft_mem, i), kefir_ast_type_unsigned_long_long());
    }

    for (kefir_float32_t f = -100.0f; f < 100.0f; f += 0.01f) {
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_float(&kft_mem, f), kefir_ast_type_float());
        ASSERT_CONSTANT(&kft_mem, &context, kefir_ast_new_constant_double(&kft_mem, (kefir_float64_t) f), kefir_ast_type_double());
    }

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_CONSTANT

#define ASSERT_STRING_LITERAL(_mem, _context, _literal) \
    do { \
        struct kefir_ast_string_literal *literal = kefir_ast_new_string_literal( \
            (_mem), &(_context)->global->symbols, (_literal)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(literal))); \
        ASSERT(literal->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(literal->base.properties.type, kefir_ast_type_array((_mem), \
            &(_context)->global->type_bundle, kefir_ast_type_char(), strlen((_literal)), NULL))); \
        ASSERT(literal->base.properties.expression_props.constant_expression); \
        ASSERT(!literal->base.properties.expression_props.lvalue); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(literal)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_string_literals, "AST node analysis - string literals")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_context_init(&kft_mem, &global_context, &context));

    ASSERT_STRING_LITERAL(&kft_mem, &context, "");
    ASSERT_STRING_LITERAL(&kft_mem, &context, "1");
    ASSERT_STRING_LITERAL(&kft_mem, &context, "abc");
    ASSERT_STRING_LITERAL(&kft_mem, &context, "Hello, world!");
    ASSERT_STRING_LITERAL(&kft_mem, &context, "\0");
    ASSERT_STRING_LITERAL(&kft_mem, &context, "\0\0\0\t");
    ASSERT_STRING_LITERAL(&kft_mem, &context, "\n\n\n\taaa");
    ASSERT_STRING_LITERAL(&kft_mem, &context, "    Hello,\n\tcruel\n\n\n  \t world\n!");

    ASSERT_OK(kefir_ast_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE