/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"
#include "declarator_analysis.h"

DEFINE_CASE(ast_declarator_analysis10, "AST declarator analysis - pointer declarators #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(&kft_mem, &specifiers, 3, kefir_ast_type_qualifier_const(&kft_mem),
                                kefir_ast_type_specifier_char(&kft_mem),
                                kefir_ast_storage_class_specifier_register(&kft_mem)));

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_pointer(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "pointer_variable1"));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator->pointer.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_VOLATILE));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    kefir_size_t alignment = 0;
    const char *identifier = NULL;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator, &identifier, &type, &storage,
                                            &function_specifier, &alignment));

    ASSERT(strcmp(identifier, "pointer_variable1") == 0);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(
        type,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle,
            kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                                   kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                            (struct kefir_ast_type_qualification){.constant = true})),
            (struct kefir_ast_type_qualification){.volatile_type = true})));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 0);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis11, "AST declarator analysis - pointer declarators #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct1", true);
    ASSERT(specifier1 != NULL);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "a"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));
    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry2, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "b"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(
        &kft_mem, &specifiers, 5, kefir_ast_storage_class_specifier_thread_local(&kft_mem),
        kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_type_specifier_struct(&kft_mem, specifier1),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16))),
        kefir_ast_storage_class_specifier_extern(&kft_mem)));

    struct kefir_ast_declarator *declarator =
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, context->symbols, NULL));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator->pointer.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_CONST));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator->pointer.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_RESTRICT));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct1", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "a", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "b",
                                          kefir_ast_type_signed_long_long(), NULL));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    kefir_size_t alignment = 0;
    const char *identifier = NULL;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator, &identifier, &type, &storage,
                                            &function_specifier, &alignment));

    ASSERT(identifier == NULL);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(
        type, kefir_ast_type_qualified(
                  &kft_mem, context->type_bundle,
                  kefir_ast_type_pointer(
                      &kft_mem, context->type_bundle,
                      kefir_ast_type_qualified(&kft_mem, context->type_bundle, type1,
                                               (struct kefir_ast_type_qualification){.volatile_type = true})),
                  (struct kefir_ast_type_qualification){.restricted = true, .constant = true})));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 16);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis12, "AST declarator analysis - array declarators #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(&kft_mem, &specifiers, 3, kefir_ast_type_qualifier_const(&kft_mem),
                                kefir_ast_type_specifier_char(&kft_mem),
                                kefir_ast_storage_class_specifier_auto(&kft_mem)));

    struct kefir_ast_declarator *declarator =
        kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL,
                                   kefir_ast_declarator_identifier(&kft_mem, context->symbols, "array1"));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_VOLATILE));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    kefir_size_t alignment = 0;
    const char *identifier = NULL;
    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator, &identifier, &type, &storage,
                                            &function_specifier, &alignment));

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        &(struct kefir_ast_type_qualification){.volatile_type = true});

    ASSERT(strcmp(identifier, "array1") == 0);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, type1));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 0);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis13, "AST declarator analysis - array declarators #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(&kft_mem, &specifiers, 3, kefir_ast_type_qualifier_const(&kft_mem),
                                kefir_ast_type_specifier_signed(&kft_mem),
                                kefir_ast_storage_class_specifier_extern(&kft_mem)));

    struct kefir_ast_declarator *declarator1 =
        kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED, NULL,
                                   kefir_ast_declarator_identifier(&kft_mem, context->symbols, "array_one"));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator1->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_RESTRICT));

    struct kefir_ast_declarator *declarator2 =
        kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED, NULL,
                                   kefir_ast_declarator_identifier(&kft_mem, context->symbols, NULL));
    declarator2->array.static_array = true;
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator2->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_RESTRICT));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator2->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_CONST));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    kefir_size_t alignment = 0;
    const char *identifier = NULL;

    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator1, &identifier, &type, &storage,
                                            &function_specifier, &alignment));
    const struct kefir_ast_type *type1 =
        kefir_ast_type_vlen_array(&kft_mem, context->type_bundle,
                                  kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                           (struct kefir_ast_type_qualification){.constant = true}),
                                  NULL, &(struct kefir_ast_type_qualification){.restricted = true});

    ASSERT(strcmp(identifier, "array_one") == 0);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, type1));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 0);

    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator2, &identifier, &type, &storage,
                                            &function_specifier, &alignment));
    const struct kefir_ast_type *type2 = kefir_ast_type_vlen_array_static(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL, &(struct kefir_ast_type_qualification){.restricted = true, .constant = true});

    ASSERT(identifier == NULL);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, type2));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 0);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator1));
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator2));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis14, "AST declarator analysis - array declarators #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(
        &kft_mem, &specifiers, 4, kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_type_specifier_signed(&kft_mem),
        kefir_ast_storage_class_specifier_extern(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 32)))));

    struct kefir_ast_declarator *declarator1 = kefir_ast_declarator_array(
        &kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)),
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, NULL));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator1->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_CONST));

    struct kefir_ast_declarator *declarator2 = kefir_ast_declarator_array(
        &kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 20)),
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "another_array"));
    declarator2->array.static_array = true;
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator2->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_VOLATILE));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator2->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_CONST));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    kefir_size_t alignment = 0;
    const char *identifier = NULL;

    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator1, &identifier, &type, &storage,
                                            &function_specifier, &alignment));
    const struct kefir_ast_type *type1 =
        kefir_ast_type_array(&kft_mem, context->type_bundle,
                             kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                      (struct kefir_ast_type_qualification){.constant = true}),
                             kefir_ast_constant_expression_integer(&kft_mem, 10),
                             &(struct kefir_ast_type_qualification){.restricted = true});

    ASSERT(identifier == NULL);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, type1));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 32);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator1));
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator2));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis15, "AST declarator analysis - array declarators #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    REQUIRE_OK(kefir_ast_local_context_declare_external(&kft_mem, &local_context, "variable",
                                                        kefir_ast_type_signed_int(), NULL, NULL));

    struct kefir_ast_declarator_specifier_list specifiers;
    ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    ASSERT_OK(append_specifiers(&kft_mem, &specifiers, 3, kefir_ast_type_qualifier_const(&kft_mem),
                                kefir_ast_type_specifier_signed(&kft_mem),
                                kefir_ast_storage_class_specifier_extern(&kft_mem)));

    struct kefir_ast_declarator *declarator1 = kefir_ast_declarator_array(
        &kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "variable")),
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, NULL));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator1->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_CONST));

    struct kefir_ast_declarator *declarator2 = kefir_ast_declarator_array(
        &kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "variable")),
        kefir_ast_declarator_identifier(&kft_mem, context->symbols, "another_array"));
    declarator2->array.static_array = true;
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator2->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_VOLATILE));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &declarator2->array.type_qualifiers,
                                                   KEFIR_AST_TYPE_QUALIFIER_CONST));

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage;
    kefir_ast_function_specifier_t function_specifier;
    kefir_size_t alignment = 0;
    const char *identifier = NULL;

    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator1, &identifier, &type, &storage,
                                            &function_specifier, &alignment));
    const struct kefir_ast_type *type1 =
        kefir_ast_type_vlen_array(&kft_mem, context->type_bundle,
                                  kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                           (struct kefir_ast_type_qualification){.constant = true}),
                                  KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "variable")),
                                  &(struct kefir_ast_type_qualification){.restricted = true});

    ASSERT(identifier == NULL);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, type1));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 0);

    ASSERT_OK(kefir_ast_analyze_declaration(&kft_mem, context, &specifiers, declarator2, &identifier, &type, &storage,
                                            &function_specifier, &alignment));
    const struct kefir_ast_type *type2 = kefir_ast_type_vlen_array_static(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "variable")),
        &(struct kefir_ast_type_qualification){.volatile_type = true, .constant = true});

    ASSERT(strcmp(identifier, "another_array") == 0);
    ASSERT(type != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(type, type2));
    ASSERT(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(alignment == 0);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator1));
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, declarator2));
    ASSERT_OK(kefir_ast_declarator_specifier_list_free(&kft_mem, &specifiers));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
