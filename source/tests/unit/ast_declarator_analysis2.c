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
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/test/util.h"
#include "declarator_analysis.h"

DEFINE_CASE(ast_declarator_analysis7, "AST declarator analysis - struct declarators #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle,
                                 kefir_ast_type_incomplete_structure(&kft_mem, context->type_bundle, "struct1"),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
        kefir_ast_storage_class_specifier_auto(&kft_mem), kefir_ast_type_qualifier_const(&kft_mem),
        kefir_ast_type_specifier_struct(
            &kft_mem, kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct1", false)));

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct2", true);
    ASSERT(specifier1 != NULL);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_short(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "x"), NULL));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "y"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "z"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_structure_specifier *specifier2 =
        kefir_ast_structure_specifier_init(&kft_mem, context->symbols, "struct3", true);
    ASSERT(specifier2 != NULL);
    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry2, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "a"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier2, entry2));
    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry3, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "b"), NULL));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry3, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "c"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier2, entry3));

    struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry4->declaration.specifiers,
                                                         kefir_ast_type_specifier_struct(&kft_mem, specifier2)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry4, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "w"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry4));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct2", &struct_type2);
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type2, "x",
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(
        &kft_mem, context->symbols, struct_type2, "y",
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL, kefir_ast_constant_expression_integer(&kft_mem, 5)));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type2, "z",
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(),
                                 (struct kefir_ast_type_qualification){.constant = true}),
        NULL));

    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct3", &struct_type3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "a", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "b",
                                          kefir_ast_type_signed_long_long(), NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context->symbols, struct_type3, "c",
                                             kefir_ast_type_signed_long_long(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type2, "w", type3, NULL));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, type2,
                                                    (struct kefir_ast_type_qualification){.volatile_type = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0,
                           4, kefir_ast_type_qualifier_volatile(&kft_mem),
                           kefir_ast_storage_class_specifier_extern(&kft_mem),
                           kefir_ast_type_specifier_struct(&kft_mem, specifier1),
                           kefir_ast_storage_class_specifier_thread_local(&kft_mem));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis8, "AST declarator analysis - struct declarators #2") {
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
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(
        &kft_mem, &entry1->declaration.specifiers,
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 16)))));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field1"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry2, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field2"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            &kft_mem, KEFIR_AST_OPERATION_ADD, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))))));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry3, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field3"),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));

    struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    struct kefir_ast_structure_specifier *specifier2 =
        kefir_ast_structure_specifier_init(&kft_mem, context->symbols, NULL, true);
    ASSERT(specifier2 != NULL);
    struct kefir_ast_structure_declaration_entry *entry2_1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2_1->declaration.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(
        &kft_mem, &entry2_1->declaration.specifiers,
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 8)))));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry2_1, kefir_ast_declarator_identifier(&kft_mem, context->symbols, "field4"), NULL));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier2, entry2_1));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry4->declaration.specifiers,
                                                         kefir_ast_type_specifier_struct(&kft_mem, specifier2)));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry4));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_union(&kft_mem, context->type_bundle, "struct1", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type1, "field1", kefir_ast_type_unsigned_long_long(),
        kefir_ast_alignment_const_expression(&kft_mem, kefir_ast_constant_expression_integer(&kft_mem, 16))));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context->symbols, struct_type1, "field2",
                                             kefir_ast_type_unsigned_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context->symbols, struct_type1, "field3",
                                             kefir_ast_type_signed_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 5)));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type2);
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type2, "field4", kefir_ast_type_float(),
        kefir_ast_alignment_const_expression(&kft_mem, kefir_ast_constant_expression_integer(&kft_mem, 8))));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, NULL, type2, NULL));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, type1, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 32, 3,
        kefir_ast_type_specifier_union(&kft_mem, specifier1), kefir_ast_storage_class_specifier_static(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 32))));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis9, "AST declarator analysis - enum declarators") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_enum_specifier *specifier1 =
        kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum1", false);
    ASSERT(specifier1 != NULL);

    const struct kefir_ast_type *type1 = kefir_ast_type_incomplete_enumeration(
        &kft_mem, context->type_bundle, "enum1", context->type_traits->underlying_enumeration_type);

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, type1, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_enum(&kft_mem, specifier1),
                           kefir_ast_storage_class_specifier_extern(&kft_mem));

    struct kefir_ast_enum_specifier *specifier2 =
        kefir_ast_enum_specifier_init(&kft_mem, context->symbols, "enum2", true);
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier2, context->symbols, "A", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier2, context->symbols, "B",
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier2, context->symbols, "C",
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier2, context->symbols, "D", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier2, context->symbols, "CONST1",
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 200))));

    struct kefir_ast_enum_type *enum_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_enumeration(
        &kft_mem, context->type_bundle, "enum2", context->type_traits->underlying_enumeration_type, &enum_type2);
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type2, "A"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type2, "B",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type2, "C",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 2)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type2, "D"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type2, "CONST1",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 200)));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, type2,
                                 (struct kefir_ast_type_qualification){.volatile_type = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 4,
        kefir_ast_storage_class_specifier_thread_local(&kft_mem), kefir_ast_type_specifier_enum(&kft_mem, specifier2),
        kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_storage_class_specifier_static(&kft_mem));

    struct kefir_ast_enum_specifier *specifier3 = kefir_ast_enum_specifier_init(&kft_mem, context->symbols, NULL, true);
    ASSERT_OK(kefir_ast_enum_specifier_append(
        &kft_mem, specifier3, context->symbols, "ONE",
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            &kft_mem, KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -1))))));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier3, context->symbols, "TWO", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier3, context->symbols, "THREE", NULL));
    ASSERT_OK(kefir_ast_enum_specifier_append(&kft_mem, specifier3, context->symbols, "TEN",
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(&kft_mem, 10))));

    struct kefir_ast_enum_type *enum_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_enumeration(
        &kft_mem, context->type_bundle, NULL, context->type_traits->underlying_enumeration_type, &enum_type3);
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type3, "ONE",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type3, "TWO"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type3, "THREE"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum_type3, "TEN",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 10)));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type3));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, type3,
                                 (struct kefir_ast_type_qualification){.constant = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 4, 3,
        kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_type_specifier_enum(&kft_mem, specifier3),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4))));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
