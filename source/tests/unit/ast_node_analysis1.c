#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"

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
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_bool(&kft_mem, false), kefir_ast_type_bool());
    ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_bool(&kft_mem, true), kefir_ast_type_bool());

    for (kefir_char_t i = KEFIR_CHAR_MIN; i < KEFIR_CHAR_MAX; i++) {
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_char(&kft_mem, i), kefir_ast_type_char());
    }

    for (kefir_int_t i = -1000; i < 1000; i++) {
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_int(&kft_mem, i), kefir_ast_type_signed_int());
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_long(&kft_mem, i), kefir_ast_type_signed_long());
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_long_long(&kft_mem, i), kefir_ast_type_signed_long_long());
    }

    for (kefir_uint_t i = 0; i < 10000; i++) {
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_uint(&kft_mem, i), kefir_ast_type_unsigned_int());
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_ulong(&kft_mem, i), kefir_ast_type_unsigned_long());
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_ulong_long(&kft_mem, i), kefir_ast_type_unsigned_long_long());
    }

    for (kefir_float32_t f = -100.0f; f < 100.0f; f += 0.01f) {
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_float(&kft_mem, f), kefir_ast_type_float());
        ASSERT_CONSTANT(&kft_mem, context, kefir_ast_new_constant_double(&kft_mem, (kefir_float64_t) f), kefir_ast_type_double());
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_CONSTANT

#define ASSERT_STRING_LITERAL(_mem, _context, _literal) \
    do { \
        struct kefir_ast_string_literal *literal = kefir_ast_new_string_literal( \
            (_mem), (_context)->symbols, (_literal)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(literal))); \
        ASSERT(literal->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(literal->base.properties.type, kefir_ast_type_array((_mem), \
            (_context)->type_bundle, kefir_ast_type_char(), strlen((_literal)), NULL))); \
        ASSERT(literal->base.properties.expression_props.constant_expression); \
        ASSERT(!literal->base.properties.expression_props.lvalue); \
        ASSERT(strcmp(literal->base.properties.expression_props.string_literal, (_literal)) == 0); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(literal)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_string_literals, "AST node analysis - string literals")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_STRING_LITERAL(&kft_mem, context, "");
    ASSERT_STRING_LITERAL(&kft_mem, context, "1");
    ASSERT_STRING_LITERAL(&kft_mem, context, "abc");
    ASSERT_STRING_LITERAL(&kft_mem, context, "Hello, world!");
    ASSERT_STRING_LITERAL(&kft_mem, context, "\0");
    ASSERT_STRING_LITERAL(&kft_mem, context, "\0\0\0\t");
    ASSERT_STRING_LITERAL(&kft_mem, context, "\n\n\n\taaa");
    ASSERT_STRING_LITERAL(&kft_mem, context, "    Hello,\n\tcruel\n\n\n  \t world\n!");

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_STRING_LITERAL

#define ASSERT_IDENTIFIER_LITERAL(_mem, _context, _identifier, _type, _constant, _lvalue) \
    do { \
        struct kefir_ast_identifier *identifier = kefir_ast_new_identifier( \
            (_mem), (_context)->symbols, (_identifier)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(identifier))); \
        ASSERT(identifier->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_type_lvalue_conversion( \
            identifier->base.properties.type), (_type))); \
        ASSERT(identifier->base.properties.expression_props.constant_expression == (_constant)); \
        ASSERT(identifier->base.properties.expression_props.lvalue == (_lvalue)); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(identifier)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_identifiers, "AST node analysis - identifiers")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *function1_type = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_void(), "func1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        "", kefir_ast_type_unsigned_char(), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *function2_type = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_unsigned_short(), "func2", &function2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2,
        "param", NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var1",
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(), (struct kefir_ast_type_qualification) {
            .constant = true,
            .restricted = false,
            .volatile_type = false
        }), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var2",
        kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        function1_type));
    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
        function2_type));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "X",
        kefir_ast_constant_expression_integer(&kft_mem, 100), context->type_traits->underlying_enumeration_type));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "Y",
        kefir_ast_constant_expression_integer(&kft_mem, -150), context->type_traits->underlying_enumeration_type));

    ASSERT_IDENTIFIER_LITERAL(&kft_mem, context, "var1", kefir_ast_type_signed_int(), false, true);
    ASSERT_IDENTIFIER_LITERAL(&kft_mem, context, "var2", kefir_ast_type_float(), false, true);
    ASSERT_IDENTIFIER_LITERAL(&kft_mem, context, "func1", function1_type, false, false);
    ASSERT_IDENTIFIER_LITERAL(&kft_mem, context, "func2", function2_type, false, false);
    ASSERT_IDENTIFIER_LITERAL(&kft_mem, context, "X", context->type_traits->underlying_enumeration_type, true, false);
    ASSERT_IDENTIFIER_LITERAL(&kft_mem, context, "Y", context->type_traits->underlying_enumeration_type, true, false);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_IDENTIFIER_LITERAL

#define ASSERT_ARRAY_SUBSCRIPT(_mem, _context, _identifier, _index, _type) \
    do { \
        struct kefir_ast_array_subscript *subscript = kefir_ast_new_array_subscript( \
            (_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_identifier))), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int((_mem), (_index)))); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(subscript))); \
        ASSERT(subscript->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(subscript->base.properties.type, (_type))); \
        ASSERT(!subscript->base.properties.expression_props.constant_expression); \
        ASSERT(subscript->base.properties.expression_props.lvalue); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(subscript)); \
        \
        struct kefir_ast_array_subscript *subscript2 = kefir_ast_new_array_subscript( \
            (_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int((_mem), (_index))), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_identifier)))); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(subscript2))); \
        ASSERT(subscript2->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(subscript2->base.properties.type, (_type))); \
        ASSERT(!subscript2->base.properties.expression_props.constant_expression); \
        ASSERT(subscript2->base.properties.expression_props.lvalue); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(subscript2)); \
    } while (0)

#define ASSERT_ARRAY_SUBSCRIPT2(_mem, _context, _identifier, _index1, _index2, _type) \
    do { \
        struct kefir_ast_array_subscript *subscript = kefir_ast_new_array_subscript( \
            (_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript( \
                (_mem), \
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_identifier))), \
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int((_mem), (_index1))))), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int((_mem), (_index2)))); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(subscript))); \
        ASSERT(subscript->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(subscript->base.properties.type, (_type))); \
        ASSERT(!subscript->base.properties.expression_props.constant_expression); \
        ASSERT(subscript->base.properties.expression_props.lvalue); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(subscript)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_array_subscripts, "AST node analysis - array subscripts")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *array1 = kefir_ast_type_pointer(&kft_mem, context->type_bundle,
        kefir_ast_type_char());
    const struct kefir_ast_type *array2 = kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle,
        kefir_ast_type_signed_short(), NULL);
    const struct kefir_ast_type *array3 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        kefir_ast_type_signed_int(), 256, NULL);
    const struct kefir_ast_type *array4 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_long()), 10, NULL);
    const struct kefir_ast_type *array5 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_long_long(), NULL), 10, NULL);
    const struct kefir_ast_type *array6 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(), 12, NULL), 12, NULL);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var1",
        array1, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var2",
        array2, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var3",
        array3, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var4",
        array4, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var5",
        array5, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var6",
        array6, NULL));

    for (kefir_size_t i = 0; i < 10; i++) {
        ASSERT_ARRAY_SUBSCRIPT(&kft_mem, context, "var1", i, kefir_ast_type_char());
        ASSERT_ARRAY_SUBSCRIPT(&kft_mem, context, "var2", i, kefir_ast_type_signed_short());
        ASSERT_ARRAY_SUBSCRIPT(&kft_mem, context, "var3", i, kefir_ast_type_signed_int());
        ASSERT_ARRAY_SUBSCRIPT(&kft_mem, context, "var4", i,
            kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_long()));
        ASSERT_ARRAY_SUBSCRIPT(&kft_mem, context, "var5", i,
            kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_long_long(), NULL));
        ASSERT_ARRAY_SUBSCRIPT(&kft_mem, context, "var6", i,
            kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(), 12, NULL));

        ASSERT_ARRAY_SUBSCRIPT2(&kft_mem, context, "var4", i, i + 100, kefir_ast_type_signed_long());
        ASSERT_ARRAY_SUBSCRIPT2(&kft_mem, context, "var5", i, i + 100, kefir_ast_type_signed_long_long());
        ASSERT_ARRAY_SUBSCRIPT2(&kft_mem, context, "var6", i, i + 100, kefir_ast_type_unsigned_long());
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_ARRAY_SUBSCRIPT
#undef ASSERT_ARRAY_SUBSCRIPT2

#define ASSERT_STRUCT_MEMBER(_mem, _context, _identifier, _field, _type) \
    do { \
        struct kefir_ast_struct_member *member = kefir_ast_new_struct_member( \
            (_mem), (_context)->symbols, \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_identifier))), \
            (_field)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(member))); \
        ASSERT(member->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(member->base.properties.type, (_type))); \
        ASSERT(!member->base.properties.expression_props.constant_expression); \
        ASSERT(member->base.properties.expression_props.lvalue); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(member)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_struct_members, "AST node analysis - struct members")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct1 = NULL;
    const struct kefir_ast_type *struct1_type = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "type1", &struct1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "field1",
        kefir_ast_type_unsigned_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "field2",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long()), NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context->symbols, struct1, "field3",
        kefir_ast_type_signed_int(), NULL, kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "field4",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_float(), NULL), NULL));

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1",
        struct1_type, NULL, NULL));
    
    ASSERT_STRUCT_MEMBER(&kft_mem, context, "var1", "field1", kefir_ast_type_unsigned_char());
    ASSERT_STRUCT_MEMBER(&kft_mem, context, "var1", "field2",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long()));
    ASSERT_STRUCT_MEMBER(&kft_mem, context, "var1", "field3", kefir_ast_type_signed_int());
    ASSERT_STRUCT_MEMBER(&kft_mem, context, "var1", "field4",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_float(), NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_STRUCT_MEMBER

#define ASSERT_INDIRECT_STRUCT_MEMBER(_mem, _context, _identifier, _field, _type) \
    do { \
        struct kefir_ast_struct_member *member = kefir_ast_new_struct_indirect_member( \
            (_mem), (_context)->symbols, \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_identifier))), \
            (_field)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(member))); \
        ASSERT(member->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(member->base.properties.type, (_type))); \
        ASSERT(!member->base.properties.expression_props.constant_expression); \
        ASSERT(member->base.properties.expression_props.lvalue); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(member)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_indirect_struct_members, "AST node analysis - indirect struct members")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct1 = NULL;
    const struct kefir_ast_type *struct1_type = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "type1", &struct1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "field1",
        kefir_ast_type_unsigned_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "field2",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long()), NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context->symbols, struct1, "field3",
        kefir_ast_type_signed_int(), NULL, kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct1, "field4",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_float(), NULL), NULL));

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, struct1_type), NULL, NULL));

    ASSERT_INDIRECT_STRUCT_MEMBER(&kft_mem, context, "var1", "field1", kefir_ast_type_unsigned_char());
    ASSERT_INDIRECT_STRUCT_MEMBER(&kft_mem, context, "var1", "field2",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long()));
    ASSERT_INDIRECT_STRUCT_MEMBER(&kft_mem, context, "var1", "field3", kefir_ast_type_signed_int());
    ASSERT_INDIRECT_STRUCT_MEMBER(&kft_mem, context, "var1", "field4",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_float(), NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_INDIRECT_STRUCT_MEMBER

#define ASSERT_FUNCTION_CALL(_mem, _context, _id, _type) \
    do { \
        struct kefir_ast_function_call *call1 = kefir_ast_new_function_call( \
            (_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_id)))); \
        ASSERT_OK(kefir_ast_function_call_append((_mem), call1, \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int((_mem), 0)))); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(call1))); \
        ASSERT(call1->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(call1->base.properties.type, (_type))); \
        ASSERT(!call1->base.properties.expression_props.constant_expression); \
        ASSERT(!call1->base.properties.expression_props.lvalue); \
        ASSERT(!call1->base.properties.expression_props.addressable); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(call1)); \
    } while (0)

#define ASSERT_FUNCTION_CALL_NOK(_mem, _context, _id) \
    do { \
        struct kefir_ast_function_call *call1 = kefir_ast_new_function_call( \
            (_mem), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier((_mem), (_context)->symbols, (_id)))); \
        ASSERT_OK(kefir_ast_function_call_append((_mem), call1, \
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int((_mem), 0)))); \
        ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(call1))); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(call1)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_function_calls, "AST node analysis - function calls")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *function1_type = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_signed_long_long(), "func1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function1,
        "", kefir_ast_type_unsigned_int(), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *function2_type = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_signed_long_long(), "func3", &function2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2,
        "", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function2,
        "", kefir_ast_type_float(), NULL));

    const struct kefir_ast_type *function1_ptr_type = kefir_ast_type_pointer(&kft_mem, context->type_bundle,
        function1_type);

    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        function1_type));
    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        function2_type));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "func2",
        function1_ptr_type, NULL, NULL));

    ASSERT_FUNCTION_CALL(&kft_mem, context, "func1", kefir_ast_type_signed_long_long());
    ASSERT_FUNCTION_CALL(&kft_mem, context, "func2", kefir_ast_type_signed_long_long());
    ASSERT_FUNCTION_CALL_NOK(&kft_mem, context, "func3");

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_FUNCTION_CALL

#define ASSERT_UNARY_OPERATION(_mem, _context, _oper, _arg, _type, _constant, _lvalue, _addresable) \
    do { \
        struct kefir_ast_unary_operation *oper = kefir_ast_new_unary_operation( \
            (_mem), (_oper), \
            KEFIR_AST_NODE_BASE((_arg))); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper))); \
        ASSERT(oper->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, (_type))); \
        ASSERT(oper->base.properties.expression_props.constant_expression == (_constant)); \
        ASSERT(oper->base.properties.expression_props.lvalue == (_lvalue)); \
        ASSERT(oper->base.properties.expression_props.addressable == (_addresable)); \
        ASSERT(!oper->base.properties.expression_props.bitfield); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper)); \
    } while (0)

#define ASSERT_UNARY_ARITH_OPERATION(_mem, _context, _oper) \
    do { \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_bool((_mem), false), \
        kefir_ast_type_signed_int(), \
        true, false, false); \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_char((_mem), 'a'), \
        kefir_ast_type_signed_int(), \
        true, false, false); \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_int((_mem), -100), \
        kefir_ast_type_signed_int(), \
        true, false, false); \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_uint((_mem), 100), \
        kefir_ast_type_unsigned_int(), \
        true, false, false); \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_long((_mem), -1000), \
        kefir_ast_type_signed_long(), \
        true, false, false); \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_ulong((_mem), 1000), \
        kefir_ast_type_unsigned_long(), \
        true, false, false); \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_long_long((_mem), -1000), \
        kefir_ast_type_signed_long_long(), \
        true, false, false); \
    ASSERT_UNARY_OPERATION((_mem), (_context), (_oper), \
        kefir_ast_new_constant_ulong_long((_mem), 1000), \
        kefir_ast_type_unsigned_long_long(), \
        true, false, false); \
    } while (0)

DEFINE_CASE(ast_node_analysis_unary_operation_arithmetic, "AST node analysis - unary arithmetic operations")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_UNARY_ARITH_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PLUS);
    ASSERT_UNARY_ARITH_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_NEGATE);
    ASSERT_UNARY_ARITH_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_INVERT);

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PLUS,
        kefir_ast_new_constant_float(&kft_mem, 3.14f),
        kefir_ast_type_float(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PLUS,
        kefir_ast_new_constant_double(&kft_mem, 2.71828),
        kefir_ast_type_double(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_NEGATE,
        kefir_ast_new_constant_float(&kft_mem, 3.14f),
        kefir_ast_type_float(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_NEGATE,
        kefir_ast_new_constant_double(&kft_mem, 2.71828),
        kefir_ast_type_double(),
        true, false, false);

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_bool(&kft_mem, false),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_char(&kft_mem, 'a'),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_int(&kft_mem, -100),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_uint(&kft_mem, 100),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_long(&kft_mem, -1000),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_ulong(&kft_mem, 1000),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_long_long(&kft_mem, -1000),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
        kefir_ast_new_constant_ulong_long(&kft_mem, 1000),
        kefir_ast_type_signed_int(),
        true, false, false);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_UNARY_ARITH_OPERATION

DEFINE_CASE(ast_node_analysis_unary_operation_address, "AST node analysis - unary address operations")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 = kefir_ast_type_pointer(&kft_mem, context->type_bundle,
        kefir_ast_type_char());
    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        kefir_ast_type_float(), 100, NULL);

    struct kefir_ast_function_type *function3 = NULL;
    const struct kefir_ast_type *function_type3 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_void(), "func1", &function3);

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var1",
        kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var2",
        type1, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var3",
        type2, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &local_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        function_type3));

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ADDRESS,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int()),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ADDRESS,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ADDRESS,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var3"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, type2),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ADDRESS,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "func1"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, function_type3),
        false, false, false);
    
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_unary_operation_indirect, "AST node analysis - unary indirect operations")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;
    const struct kefir_ast_type *type1 = kefir_ast_type_pointer(&kft_mem, context->type_bundle,
        kefir_ast_type_char());
    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        kefir_ast_type_float(), 100, NULL);

    struct kefir_ast_function_type *function3 = NULL;
    const struct kefir_ast_type *function_type3 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_double(), "func1", &function3);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, function3,
        "", kefir_ast_type_bool(), NULL));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var1",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int()), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var2",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var3",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, type2), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var4",
        type2, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var5",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, function_type3), NULL));

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_INDIRECTION,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"),
        kefir_ast_type_signed_int(),
        false, true, true);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_INDIRECTION,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"),
        type1,
        false, true, true);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_INDIRECTION,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var3"),
        type2,
        false, true, true);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_INDIRECTION,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var4"),
        kefir_ast_type_float(),
        false, true, true);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_INDIRECTION,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var5"),
        function_type3,
        false, false, true);
    
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_unary_operation_incdec, "AST node analysis - unary increment/decrement operations")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var1",
        kefir_ast_type_bool(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var2",
        kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var3",
        kefir_ast_type_unsigned_long_long(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var4",
        kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "var5",
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()), NULL));

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"),
        kefir_ast_type_bool(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"),
        kefir_ast_type_signed_int(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var3"),
        kefir_ast_type_unsigned_long_long(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var4"),
        kefir_ast_type_float(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var5"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
        false, false, false);

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"),
        kefir_ast_type_bool(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"),
        kefir_ast_type_signed_int(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var3"),
        kefir_ast_type_unsigned_long_long(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var4"),
        kefir_ast_type_float(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var5"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
        false, false, false);

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"),
        kefir_ast_type_bool(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"),
        kefir_ast_type_signed_int(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var3"),
        kefir_ast_type_unsigned_long_long(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var4"),
        kefir_ast_type_float(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var5"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
        false, false, false);

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var1"),
        kefir_ast_type_bool(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var2"),
        kefir_ast_type_signed_int(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var3"),
        kefir_ast_type_unsigned_long_long(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var4"),
        kefir_ast_type_float(),
        false, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_PREFIX_INCREMENT,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "var5"),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
        false, false, false);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_unary_operation_sizeof, "AST node analysis - unary sizeof operations")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "x",
        kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "y",
        kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), 1, NULL), NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "z",
        kefir_ast_type_vlen_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 128)), NULL), NULL));

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_SIZEOF,
        kefir_ast_new_constant_bool(&kft_mem, false),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_SIZEOF,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "x"),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_SIZEOF,
        kefir_ast_new_string_literal(&kft_mem, context->symbols, "Hello, world!"),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_SIZEOF,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "y"),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_SIZEOF,
        kefir_ast_new_identifier(&kft_mem, context->symbols, "z"),
        kefir_ast_type_signed_int(),
        false, false, false);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_unary_operation_alignof, "AST node analysis - unary alignof operations")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ALIGNOF,
        kefir_ast_new_type_name(&kft_mem, kefir_ast_type_bool()),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ALIGNOF,
        kefir_ast_new_type_name(&kft_mem, kefir_ast_type_unsigned_short()),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ALIGNOF,
        kefir_ast_new_type_name(&kft_mem, kefir_ast_type_signed_int()),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ALIGNOF,
        kefir_ast_new_type_name(&kft_mem, kefir_ast_type_unsigned_long_long()),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ALIGNOF,
        kefir_ast_new_type_name(&kft_mem, kefir_ast_type_float()),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ALIGNOF,
        kefir_ast_new_type_name(&kft_mem, kefir_ast_type_array_static(&kft_mem, context->type_bundle,
            kefir_ast_type_char(), 150, NULL)),
        kefir_ast_type_signed_int(),
        true, false, false);
    ASSERT_UNARY_OPERATION(&kft_mem, context, KEFIR_AST_OPERATION_ALIGNOF,
        kefir_ast_new_type_name(&kft_mem, kefir_ast_type_pointer(&kft_mem, context->type_bundle,
            kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                (struct kefir_ast_type_qualification){
                    .constant = true,
                    .restricted = false,
                    .volatile_type = false
                }))),
        kefir_ast_type_signed_int(),
        true, false, false);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_UNARY_OPERATION

#define ASSERT_TYPE_NAME(_mem, _context, _type) \
    do { \
        struct kefir_ast_type_name *type_name = kefir_ast_new_type_name( \
            (_mem), (_type)); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(type_name))); \
        ASSERT(type_name->base.properties.category == KEFIR_AST_NODE_CATEGORY_TYPE); \
        ASSERT(KEFIR_AST_TYPE_SAME(type_name->base.properties.type, (_type))); \
        ASSERT(!type_name->base.properties.expression_props.constant_expression); \
        ASSERT(!type_name->base.properties.expression_props.lvalue); \
        ASSERT(!type_name->base.properties.expression_props.addressable); \
        KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(type_name)); \
    } while (0)

DEFINE_CASE(ast_node_analysis_type_name, "AST node analysis - type names")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_bool());
    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_signed_char());
    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_signed_int());
    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_unsigned_long_long());
    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_float());
    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_double());
    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_TYPE_NAME(&kft_mem, context, kefir_ast_type_array(&kft_mem, context->type_bundle,
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), 256, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE