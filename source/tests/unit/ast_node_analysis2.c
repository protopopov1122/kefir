#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"

#define ASSERT_GENERIC_SELECTION(_mem, _context, _id, _type, _lvalue, _const, _addressable) \
    do { \
        struct kefir_ast_generic_selection *selection1 = kefir_ast_new_generic_selection((_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_id)))); \
        ASSERT_OK(kefir_ast_generic_selection_append((_mem), selection1, (_context)->type_traits, \
            kefir_ast_type_char(), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, "bool")))); \
        ASSERT_OK(kefir_ast_generic_selection_append((_mem), selection1, (_context)->type_traits, \
            kefir_ast_type_unsigned_int(), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float((_mem), 3.14f)))); \
        ASSERT_OK(kefir_ast_generic_selection_append((_mem), selection1, (_context)->type_traits, \
            kefir_ast_type_double(), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char((_mem), 'H')))); \
        ASSERT_OK(kefir_ast_generic_selection_append((_mem), selection1, (_context)->type_traits, \
            kefir_ast_type_pointer((_mem), (_context)->type_bundle, kefir_ast_type_void()), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint((_mem), 1556)))); \
        ASSERT_OK(kefir_ast_generic_selection_append((_mem), selection1, (_context)->type_traits, \
            NULL, \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double((_mem), 2.71)))); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(selection1))); \
        ASSERT(KEFIR_AST_TYPE_SAME(selection1->base.properties.type, (_type))); \
        ASSERT(selection1->base.properties.expression_props.lvalue == (_lvalue)); \
        ASSERT(selection1->base.properties.expression_props.constant_expression == (_const)); \
        ASSERT(selection1->base.properties.expression_props.addressable == (_addressable)); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(selection1))); \
    } while (0)

DEFINE_CASE(ast_node_analysis_generic_selections, "AST node analysis - generic selections")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "x",
        kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "y",
        kefir_ast_constant_expression_integer(&kft_mem, 100), kefir_ast_type_unsigned_int()));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "z",
        kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "w",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "q",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "r",
        kefir_ast_type_signed_long_long(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "bool",
        kefir_ast_type_bool(), NULL));

    ASSERT_GENERIC_SELECTION(&kft_mem, context, "x", kefir_ast_type_bool(), true, false, true);
    ASSERT_GENERIC_SELECTION(&kft_mem, context, "y", kefir_ast_type_float(), false, true, false);
    ASSERT_GENERIC_SELECTION(&kft_mem, context, "z", kefir_ast_type_char(), false, true, false);
    ASSERT_GENERIC_SELECTION(&kft_mem, context, "w", kefir_ast_type_unsigned_int(), false, true, false);
    ASSERT_GENERIC_SELECTION(&kft_mem, context, "q", kefir_ast_type_double(), false, true, false);
    ASSERT_GENERIC_SELECTION(&kft_mem, context, "r", kefir_ast_type_double(), false, true, false);
    
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE