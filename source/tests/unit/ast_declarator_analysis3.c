#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"
#include "declarator_analysis.h"

DEFINE_CASE(ast_declarator_analysis10, "AST declarator analysis - pointer declarators #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(&kft_mem, &specifiers, 3,
        kefir_ast_type_qualifier_const(&kft_mem),
        kefir_ast_type_specifier_char(&kft_mem),
        kefir_ast_storage_class_specifier_register(&kft_mem)));

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_pointer(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "pointer_variable1"));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator->pointer.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_VOLATILE));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    struct kefir_ast_alignment *alignment = NULL;
    const char *identifier = NULL;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers,
        declarator, &identifier, &type, &storage, &function_specifier, &alignment));

    ASSERT(strcmp(identifier, "pointer_variable1") == 0);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
        kefir_ast_type_pointer(&kft_mem, context->type_bundle,
            kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                (struct kefir_ast_type_qualification) {.constant = true})),
        (struct kefir_ast_type_qualification) {.volatile_type = true})));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == NULL);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis11, "AST declarator analysis - pointer declarators #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct1", true);
    ASSERT(specifier1 != NULL);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "a"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));
    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "b"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(&kft_mem, &specifiers, 5,
        kefir_ast_storage_class_specifier_thread_local(&kft_mem),
        kefir_ast_type_qualifier_volatile(&kft_mem),
        kefir_ast_type_specifier_struct(&kft_mem, specifier1),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16))),
        kefir_ast_storage_class_specifier_extern(&kft_mem)));

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_pointer(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, NULL));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator->pointer.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_CONST));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator->pointer.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_RESTRICT));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "struct1", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "a",
        kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "b",
        kefir_ast_type_signed_long_long(), NULL));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    struct kefir_ast_alignment *alignment = NULL;
    const char *identifier = NULL;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers,
        declarator, &identifier, &type, &storage, &function_specifier, &alignment));

    ASSERT(identifier == NULL);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
        kefir_ast_type_pointer(&kft_mem, context->type_bundle,
            kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
                (struct kefir_ast_type_qualification) {.volatile_type = true})),
        (struct kefir_ast_type_qualification) {.restricted = true, .constant = true})));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment != NULL);
    ASSERT(alignment->value == 16);
    ASSERT_OK(kefir_ast_alignment_free(&kft_mem, alignment));

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
