#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"
#include "declarator_analysis.h"

DEFINE_CASE(ast_declarator_analysis21, "AST declarator analysis - function declarators #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(&kft_mem,
        context->symbols, "param1_struct", true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_declaration *param1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "param1"), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, specifier1)));

    struct kefir_ast_declaration *param2 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "param2"), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param2->specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, kefir_ast_structure_specifier_init(&kft_mem, context->symbols,
            "param1_struct", false))));

    struct kefir_ast_declaration *param3 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param3->specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, kefir_ast_structure_specifier_init(&kft_mem, context->symbols,
            "param1_struct", false))));

    struct kefir_ast_declaration *param4 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "paramN"), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param4->specifiers,
        kefir_ast_type_specifier_double(&kft_mem)));

    struct kefir_ast_declarator_specifier_list decl_specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&decl_specifiers));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl_specifiers,
        kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_declarator *decl0 = kefir_ast_declarator_function(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "someFunc"));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl0->function.parameters, kefir_list_tail(&decl0->function.parameters),
        KEFIR_AST_NODE_BASE(param1)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl0->function.parameters, kefir_list_tail(&decl0->function.parameters),
        KEFIR_AST_NODE_BASE(param2)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl0->function.parameters, kefir_list_tail(&decl0->function.parameters),
        KEFIR_AST_NODE_BASE(param3)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl0->function.parameters, kefir_list_tail(&decl0->function.parameters),
        KEFIR_AST_NODE_BASE(param4)));
    decl0->function.ellipsis = true;

    struct kefir_ast_declarator *decl = kefir_ast_declarator_pointer(&kft_mem, decl0);

    const char *identifier = NULL;
    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_ast_function_specifier_t func_specifier = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
    kefir_size_t alignment = 0;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &decl_specifiers, decl,
        &identifier, &type, &storage, &func_specifier, &alignment));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "param1_struct", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "x", kefir_ast_type_signed_int(), NULL));

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), "someFunc", &func_type2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle,
        func_type2, "param1", type1, &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle,
        func_type2, "param2", type1, &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle,
        func_type2, NULL, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1),
            &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle,
        func_type2, "paramN", kefir_ast_type_double(),
        &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
    ASSERT_OK(kefir_ast_type_function_ellipsis(func_type2, true));

    ASSERT(KEFIR_AST_TYPE_SAME(type, type2));

    const struct kefir_ast_scoped_identifier *scoped_id1 = NULL;
    ASSERT(context->resolve_tag_identifier(context, "param1_struct", &scoped_id1) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &decl_specifiers));
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis22, "AST declarator analysis - function declarators #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(&kft_mem,
        context->symbols, "oneStructure", true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "content")), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "length"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_declaration(&kft_mem, NULL, NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, specifier1)));
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &decl1->specifiers, decl1->declarator,
        NULL, NULL, NULL, NULL, NULL));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "oneStructure", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "content", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_void(), (struct kefir_ast_type_qualification){
                .constant = true
            })), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "length", kefir_ast_type_unsigned_int(), NULL));

    struct kefir_ast_declaration *param1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "param1"), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, kefir_ast_structure_specifier_init(&kft_mem, context->symbols,
            "oneStructure", false))));

    struct kefir_ast_declaration *param2 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, NULL, NULL), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param2->specifiers,
        kefir_ast_storage_class_specifier_register(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param2->specifiers,
        kefir_ast_type_specifier_char(&kft_mem)));

    struct kefir_ast_declaration *param3_1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "num"), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param3_1->specifiers,
        kefir_ast_type_specifier_float(&kft_mem)));

    struct kefir_ast_declaration *param3 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_function(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "callback")),
        NULL);
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &param3->declarator->function.parameters,
        kefir_list_tail(&param3->declarator->function.parameters),
        KEFIR_AST_NODE_BASE(param3_1)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param3->specifiers,
        kefir_ast_storage_class_specifier_register(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param3->specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    

    struct kefir_ast_declarator_specifier_list decl2_specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&decl2_specifiers));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl2_specifiers,
        kefir_ast_type_specifier_long(&kft_mem)));

    struct kefir_ast_declarator *decl2 = kefir_ast_declarator_function(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "modify"));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl2->function.parameters, kefir_list_tail(&decl2->function.parameters),
        KEFIR_AST_NODE_BASE(param1)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl2->function.parameters, kefir_list_tail(&decl2->function.parameters),
        KEFIR_AST_NODE_BASE(param2)));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl2->function.parameters, kefir_list_tail(&decl2->function.parameters),
        KEFIR_AST_NODE_BASE(param3)));

    const char *identifier2 = NULL;
    const struct kefir_ast_type *decl_type2 = NULL;
    kefir_ast_scoped_identifier_storage_t storage2 = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_ast_function_specifier_t func_specifier2 = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
    kefir_size_t alignment2 = 0;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &decl2_specifiers, decl2,
        &identifier2, &decl_type2, &storage2, &func_specifier2, &alignment2));

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_unsigned_int(), "callback", &func_type2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type2,
        "num", kefir_ast_type_float(), &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));

    struct kefir_ast_function_type *func_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_signed_long(), "modify", &func_type3);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type3,
        "param1", type1, &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type3,
        NULL, kefir_ast_type_char(), &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER}));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type3,
        "callback", type2,
        &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER}));

    ASSERT(KEFIR_AST_TYPE_SAME(decl_type2, type3));
    
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &decl2_specifiers));
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl2));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis23, "AST declarator analysis - function declarators #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declaration *param1 = kefir_ast_new_declaration(&kft_mem,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)), NULL);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &param1->specifiers,
        kefir_ast_type_specifier_void(&kft_mem)));

    struct kefir_ast_declarator *decl1 = kefir_ast_declarator_function(&kft_mem,
        kefir_ast_declarator_pointer(&kft_mem,
            kefir_ast_declarator_identifier(&kft_mem, context->symbols, "callback")));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl1->function.parameters, kefir_list_tail(&decl1->function.parameters),
        KEFIR_AST_NODE_BASE(param1)));

    struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(&kft_mem, context->symbols,
        "callback_S", true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1, decl1, NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "payload")), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_declarator_specifier_list decl_specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&decl_specifiers));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl_specifiers,
        kefir_ast_storage_class_specifier_typedef(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl_specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, specifier1)));

    struct kefir_ast_declarator *decl = kefir_ast_declarator_identifier(&kft_mem, context->symbols, "callback_t");

    const char *identifier2 = NULL;
    const struct kefir_ast_type *decl_type2 = NULL;
    kefir_ast_scoped_identifier_storage_t storage2 = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_ast_function_specifier_t func_specifier2 = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
    kefir_size_t alignment2 = 0;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &decl_specifiers, decl,
        &identifier2, &decl_type2, &storage2, &func_specifier2, &alignment2));

    const struct kefir_ast_type *type1 = kefir_ast_type_pointer(&kft_mem, context->type_bundle,
        kefir_ast_type_void());

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, context->type_bundle,
        kefir_ast_type_unsigned_char(), NULL, &func_type2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type2,
        NULL, type1, &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN}));

    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "callback_S", &struct_type3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3,
        "callback", kefir_ast_type_pointer(&kft_mem, context->type_bundle, type2), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3,
        "payload", type1, NULL));

    ASSERT(strcmp(identifier2, "callback_t") == 0);
    ASSERT(KEFIR_AST_TYPE_SAME(decl_type2, type3));
    ASSERT(storage2 == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF);
    ASSERT(func_specifier2 == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment2 == 0);

    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &decl_specifiers));
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis24, "AST declarator analysis - structure static assertions #1")
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
        kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1,
        kefir_ast_structure_declaration_entry_alloc_assert(&kft_mem, kefir_ast_new_static_assertion(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "TRUE")))));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_declarator_specifier_list decl_specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&decl_specifiers));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl_specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, specifier1)));
    
    const char *identifier = NULL;
    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_ast_function_specifier_t func_specifier = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
    kefir_size_t alignment = 0;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &decl_specifiers, NULL,
        &identifier, &type, &storage, &func_specifier, &alignment));

    ASSERT(identifier == NULL);
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN);
    ASSERT(func_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 0);

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "structure1", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "x", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "y", kefir_ast_type_float(), NULL));

    ASSERT(KEFIR_AST_TYPE_SAME(type, type1));

    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &decl_specifiers));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis25, "AST declarator analysis - structure static assertions #2")
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
        kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1,
        kefir_ast_structure_declaration_entry_alloc_assert(&kft_mem, kefir_ast_new_static_assertion(&kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, false)),
            KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "FALSE")))));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry2,
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_declarator_specifier_list decl_specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&decl_specifiers));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl_specifiers,
        kefir_ast_type_specifier_struct(&kft_mem, specifier1)));
    
    const char *identifier = NULL;
    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_ast_function_specifier_t func_specifier = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
    kefir_size_t alignment = 0;
    ASSERT(kefir_ast_analyze_declaration(&kft_mem, context, &decl_specifiers, NULL,
        &identifier, &type, &storage, &func_specifier, &alignment) == KEFIR_STATIC_ASSERT);

    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &decl_specifiers));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
