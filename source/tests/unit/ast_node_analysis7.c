#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_node_analysis_declarations1, "AST node analysis - declarations #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "some_var1"),
        NULL);
    ASSERT(decl1 != NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16)))));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_storage_class_specifier_extern(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    const struct kefir_ast_type *type1 = kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(),
        (struct kefir_ast_type_qualification){
            .constant = true
        });
    ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
    ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
    ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "some_var1") == 0);
    ASSERT(decl1->base.properties.declaration_props.alignment == 16);

    const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "some_var1",
        &scoped_id1));
    ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
    ASSERT(scoped_id1->object.external);
    ASSERT(scoped_id1->object.alignment != NULL);
    ASSERT(scoped_id1->object.alignment->value == 16);
    ASSERT(scoped_id1->object.initializer == NULL);
    
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations2, "AST node analysis - declarations #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;
    
    struct kefir_ast_initializer *initializer1 = kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.14f)));
    
    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "declaration_one"),
        initializer1);
    ASSERT(decl1 != NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_qualifier_restrict(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4)))));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_storage_class_specifier_auto(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_qualifier_volatile(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    const struct kefir_ast_type *type1 = kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
        (struct kefir_ast_type_qualification){
            .restricted = true,
            .volatile_type = true
        });
    ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
    ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
    ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
    ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "declaration_one") == 0);
    ASSERT(decl1->base.properties.declaration_props.alignment == 4);

    const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "declaration_one",
        &scoped_id1));
    ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
    ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
    ASSERT(!scoped_id1->object.external);
    ASSERT(scoped_id1->object.alignment != NULL);
    ASSERT(scoped_id1->object.alignment->value == 4);
    ASSERT(scoped_id1->object.initializer == initializer1);

    ASSERT(scoped_id1->object.initializer->type == KEFIR_AST_INITIALIZER_EXPRESSION);
    ASSERT(scoped_id1->object.initializer->expression->klass->type == KEFIR_AST_CONSTANT);
    
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations3, "AST node analysis - declarations #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(&kft_mem, context->symbols,
        "structure1", true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field1")), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));
    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_double(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field2")), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));
    
    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "VaRiAblE123"),
        NULL);
    ASSERT(decl1 != NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, specifier1)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 8)))));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_storage_class_specifier_static(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "structure1", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "field1", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_int()), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "field2", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_double()), NULL));
    const struct kefir_ast_type *qualifier_type1 = kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
        (struct kefir_ast_type_qualification){
            .constant = true
        });
    ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, qualifier_type1));
    ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
    ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "VaRiAblE123") == 0);
    ASSERT(decl1->base.properties.declaration_props.alignment == 8);

    const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&local_context, "VaRiAblE123",
        &scoped_id1));
    ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, qualifier_type1));
    ASSERT(!scoped_id1->object.external);
    ASSERT(scoped_id1->object.alignment != NULL);
    ASSERT(scoped_id1->object.alignment->value == 8);
    ASSERT(scoped_id1->object.initializer == NULL);

    const struct kefir_ast_scoped_identifier *scoped_id2 = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_tag_identifier(&local_context, "structure1",
        &scoped_id2));
    ASSERT(scoped_id2->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id2->type, type1));
    
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations4, "AST node analysis - declarations #4")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    struct kefir_ast_context *context = &global_context.context;
    
    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "globalVariable"),
        NULL);
    ASSERT(decl1 != NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_storage_class_specifier_thread_local(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    const struct kefir_ast_type *type1 = kefir_ast_type_unsigned_long_long();
    ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
    ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL);
    ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
    ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "globalVariable") == 0);
    ASSERT(decl1->base.properties.declaration_props.alignment == 0);

    const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
    ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "globalVariable",
        &scoped_id1));
    ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
    ASSERT(!scoped_id1->object.external);
    ASSERT(scoped_id1->object.alignment != NULL);
    ASSERT(scoped_id1->object.alignment->value == 0);
    ASSERT(scoped_id1->object.initializer == NULL);
    
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_declarations5, "AST node analysis - declarations #5")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    struct kefir_ast_context *context = &global_context.context;

    struct kefir_ast_enum_specifier *specifier1 = kefir_ast_enum_specifier_init(&kft_mem, context->symbols,
        "enumeration1", true);
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "XVAL", KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 1000))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "YVAL", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier1, context->symbols,
        "AVAL", KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0))));
    
    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "counter"),
        NULL);
    ASSERT(decl1 != NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_storage_class_specifier_thread_local(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_enum(&kft_mem, specifier1)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_storage_class_specifier_static(&kft_mem)));
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(decl1)));

    struct kefir_ast_enum_type *enum_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(&kft_mem, context->type_bundle, "enumeration1",
        context->type_traits->underlying_enumeration_type, &enum_type1);
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type1, "XVAL",
        kefir_ast_constant_expression_integer(&kft_mem, 1000)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "YVAL"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type1, "AVAL",
        kefir_ast_constant_expression_integer(&kft_mem, 0)));


    ASSERT(decl1->base.properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION);
    ASSERT(KEFIR_AST_TYPE_SAME(decl1->base.properties.type, type1));
    ASSERT(decl1->base.properties.declaration_props.function == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(decl1->base.properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    ASSERT(decl1->base.properties.declaration_props.identifier != NULL);
    ASSERT(strcmp(decl1->base.properties.declaration_props.identifier, "counter") == 0);
    ASSERT(decl1->base.properties.declaration_props.alignment == 0);

    const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
    ASSERT_OK(kefir_ast_global_context_resolve_scoped_ordinary_identifier(&global_context, "counter",
        &scoped_id1));
    ASSERT(scoped_id1->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id1->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    ASSERT(scoped_id1->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id1->object.type, type1));
    ASSERT(!scoped_id1->object.external);
    ASSERT(scoped_id1->object.alignment != NULL);
    ASSERT(scoped_id1->object.alignment->value == 0);
    ASSERT(scoped_id1->object.initializer == NULL);
    
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

// TODO Implement more declaration analysis tests
