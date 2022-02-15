/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/test/unit_test.h"
#include "kefir/ast/type.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/function_declaration_context.h"
#include "kefir/ast/declarator.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_declaration_variably_modified1, "AST declarations - variably modified declarations in global scope") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));

    const struct kefir_ast_type *type1 = kefir_ast_type_vlen_array(
        &kft_mem, &global_context.type_bundle, kefir_ast_type_unsigned_short(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &global_context.symbols, "x")), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type1));

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, type1);

    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test1", type1,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test2", type2,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test3", type1,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test4", type2,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test5", type1,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test6", type2,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test7", type1,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test8", type2,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test9", type1,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test10", type2,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test11", type1,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test12", type2,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test13", type1,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(global_context.context.define_identifier(&kft_mem, &global_context.context, true, "test14", type2,
                                                        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declaration_variably_modified2, "AST declarations - variably modified declarations in local scope") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));

    const struct kefir_ast_type *type1 = kefir_ast_type_vlen_array(
        &kft_mem, &global_context.type_bundle, kefir_ast_type_unsigned_short(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &global_context.symbols, "x")), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type1));

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, type1);

    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test1", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test2", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test3", type1,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test4", type2,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test5", type1,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test6", type2,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test7", type1,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test8", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test9", type1,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test10", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test11", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test12", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test13", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_OK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test14", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declaration_variably_modified3,
            "AST declarations - variably modified declarations in function declaration scope") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    struct kefir_ast_function_declaration_context func_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    ASSERT_OK(kefir_ast_function_declaration_context_init(&kft_mem, &local_context.context, false, &func_context));

    const struct kefir_ast_type *type1 = kefir_ast_type_vlen_array(
        &kft_mem, &global_context.type_bundle, kefir_ast_type_unsigned_short(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &global_context.symbols, "x")), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type1));

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, type1);

    ASSERT_OK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test1", type1,
                                                     KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                     KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_OK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test2", type2,
                                                     KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                     KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test3", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test4", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test5", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test6", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test7", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test8", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test9", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test10", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test11", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test12", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_OK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test13", type1,
                                                     KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                     KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));
    ASSERT_OK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test14", type2,
                                                     KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                     KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, NULL));

    ASSERT_OK(kefir_ast_function_declaration_context_free(&kft_mem, &func_context));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declaration_variably_modified4, "AST declarations - variably modified declarations initializers") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    struct kefir_ast_function_declaration_context func_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    ASSERT_OK(kefir_ast_function_declaration_context_init(&kft_mem, &local_context.context, false, &func_context));

    const struct kefir_ast_type *type1 = kefir_ast_type_vlen_array(
        &kft_mem, &global_context.type_bundle, kefir_ast_type_unsigned_short(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &global_context.symbols, "x")), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type1));

    const struct kefir_ast_type *type2 = kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, type1);

    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(
        &kft_mem, &init1->list, NULL,
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
    struct kefir_ast_initializer *init2 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)));

    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test1", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init1, NULL, NULL));
    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test2", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init2, NULL, NULL));

    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test3", type1,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init1, NULL, NULL));
    ASSERT_NOK(func_context.context.define_identifier(&kft_mem, &func_context.context, true, "test4", type2,
                                                      KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                      KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init2, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test5", type1,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init1, NULL, NULL));
    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test6", type2,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init2, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test7", type2,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init2, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test8", type1,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init1, NULL, NULL));
    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test9", type2,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init2, NULL, NULL));

    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test10", type1,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init1, NULL, NULL));
    ASSERT_NOK(local_context.context.define_identifier(&kft_mem, &local_context.context, true, "test11", type2,
                                                       KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                                                       KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, init2, NULL, NULL));

    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    ASSERT_OK(kefir_ast_function_declaration_context_free(&kft_mem, &func_context));
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declaration_variably_modified_structure_field, "AST declarations - variably modified structure field") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "x", kefir_ast_type_signed_int(),
                                                        NULL, NULL, NULL));

    struct kefir_ast_structure_specifier *struct_specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, &global_context.symbols, "struct1", true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1,
        kefir_ast_declarator_array(
            &kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &global_context.symbols, "x")),
            kefir_ast_declarator_identifier(&kft_mem, &global_context.symbols, "field1")),
        NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, struct_specifier1, entry1));

    struct kefir_ast_init_declarator *init_decl = NULL;
    struct kefir_ast_declaration *decl = kefir_ast_new_single_declaration(
        &kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL), NULL, &init_decl);
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl->specifiers,
                                                         kefir_ast_type_specifier_struct(&kft_mem, struct_specifier1)));

    ASSERT_NOK(kefir_ast_analyze_declaration(&kft_mem, &global_context.context, &decl->specifiers,
                                             init_decl->declarator, NULL, NULL, NULL, NULL, NULL));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl)));

    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
