#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"
#include "declarator_analysis.h"

DEFINE_CASE(ast_declarator_analysis16, "AST declarator analysis - struct type tags #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "structure1", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "field1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "field2", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "field3", kefir_ast_type_unsigned_long_long(), NULL));

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "structure1", true);

    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field1"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field2"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry3,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field3"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));

    struct kefir_ast_struct_type *union_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_union(&kft_mem, context->type_bundle,
        "union1", &union_type2);

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_incomplete_structure(&kft_mem, context->type_bundle, "structure1"),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_struct(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "structure1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_incomplete_union(&kft_mem, context->type_bundle, "union1"),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_union(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "union1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type1,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_struct(&kft_mem, specifier1));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type2,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_union(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "union1", true)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type1,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_struct(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "structure1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type2,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_union(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "union1", false)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis17, "AST declarator analysis - union type tags #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *union_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_union(&kft_mem, context->type_bundle,
        "union1", &union_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type1,
        "field1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type1,
        "field2", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type1,
        "field3", kefir_ast_type_unsigned_long_long(), NULL));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "struct1", &struct_type2);

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "union1", true);

    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field1"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field2"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry3,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field3"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_incomplete_union(&kft_mem, context->type_bundle, "union1"),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_union(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "union1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_incomplete_structure(&kft_mem, context->type_bundle, "struct1"),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_struct(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type1,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_union(&kft_mem, specifier1));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type2,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_struct(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct1", true)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type1,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_union(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "union1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type2,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_struct(&kft_mem,
            kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct1", false)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis18, "AST declarator analysis - enum type tags #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_enum_specifier *specifier1 = kefir_ast_enum_specifier_init(&kft_mem, context->symbols,
        "enum1", true);
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "CONST_A", KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "CONST_B", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "CONST_C", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "CONST_D", KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "CONST_E", NULL));

    struct kefir_ast_enum_specifier *specifier2 = kefir_ast_enum_specifier_init(&kft_mem, context->symbols,
        "enum2", true);
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier2, context->symbols,
        "ONE_THING", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier2, context->symbols,
        "ANOTHER_THING", NULL));

    struct kefir_ast_enum_type *enum_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(&kft_mem, context->type_bundle,
        "enum1", context->type_traits->underlying_enumeration_type, &enum_type1);
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type1,
        "CONST_A", kefir_ast_constant_expression_integer(&kft_mem, 100)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1,
        "CONST_B"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1,
        "CONST_C"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type1,
        "CONST_D", kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1,
        "CONST_E"));

    struct kefir_ast_enum_type *enum_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_enumeration(&kft_mem, context->type_bundle,
        "enum2", context->type_traits->underlying_enumeration_type, &enum_type2);
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type2,
        "ONE_THING"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type2,
        "ANOTHER_THING"));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_incomplete_enumeration(&kft_mem, context->type_bundle, "enum1",
            context->type_traits->underlying_enumeration_type),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_enum(&kft_mem,
            kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_incomplete_enumeration(&kft_mem, context->type_bundle, "enum2",
            context->type_traits->underlying_enumeration_type),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_enum(&kft_mem,
            kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum2", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        type1,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_enum(&kft_mem, specifier1));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        type2,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_enum(&kft_mem, specifier2));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        type1,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_enum(&kft_mem,
            kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum1", false)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        type2,
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        0, 1,
        kefir_ast_type_specifier_enum(&kft_mem,
            kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum2", false)));

#define ASSERT_CONSTANT(_id, _value) \
    do { \
        const struct kefir_ast_scoped_identifier *scoped_identifier = NULL; \
        ASSERT_OK(context->resolve_ordinary_identifier(context, (_id), &scoped_identifier)); \
        ASSERT(scoped_identifier->klass == KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT); \
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_identifier->enum_constant.type, context->type_traits->underlying_enumeration_type)); \
        ASSERT(scoped_identifier->enum_constant.value->value.integer == (_value)); \
    } while (0)

    ASSERT_CONSTANT("CONST_A", 100);
    ASSERT_CONSTANT("CONST_B", 101);
    ASSERT_CONSTANT("CONST_C", 102);
    ASSERT_CONSTANT("CONST_D", 1);
    ASSERT_CONSTANT("ONE_THING", 0);
    ASSERT_CONSTANT("ANOTHER_THING", 1);

#undef ASSERT_CONSTANT

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
