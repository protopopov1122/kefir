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
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_ordinary_function_scope1, "AST ordinary scope - functions #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "", &function_type1);

    ASSERT_NOK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                         type1, NULL, NULL));

    type1 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "func1", &function_type1);
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type1, "x", NULL, NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type1, "y", NULL, NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type1, "z", NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        type1, NULL, NULL));
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(scoped_id->function.external);

    struct kefir_ast_function_type *function_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, &global_context.type_bundle,
                                                                 kefir_ast_type_signed_int(), "func1", &function_type2);

    ASSERT_NOK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                         type2, NULL, NULL));

    type2 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "func1", &function_type2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type2, "x",
                                                kefir_ast_type_signed_long_long(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type2, "y",
                                                kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, &global_context.type_bundle, function_type2, "z",
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()), NULL));

    ASSERT_OK(kefir_ast_global_context_define_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
                                                       type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->function.type, type2));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(!scoped_id->function.external);

    ASSERT_NOK(kefir_ast_global_context_define_static_function(&kft_mem, &global_context,
                                                               KEFIR_AST_FUNCTION_SPECIFIER_INLINE, type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_function_scope2, "AST ordinary scope - functions #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    struct kefir_ast_function_type *function_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_function(
        &kft_mem, &global_context.type_bundle,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()), "", &function_type1);

    ASSERT_NOK(kefir_ast_global_context_define_static_function(&kft_mem, &global_context,
                                                               KEFIR_AST_FUNCTION_SPECIFIER_NONE, type1, NULL, NULL));

    type1 = kefir_ast_type_function(
        &kft_mem, &global_context.type_bundle,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()), "func1", &function_type1);
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, &global_context.type_bundle, function_type1, NULL,
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_long_long(), NULL),
        NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, &global_context.type_bundle, function_type1, NULL,
        kefir_ast_type_qualified(
            &kft_mem, &global_context.type_bundle, kefir_ast_type_double(),
            (const struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = false}),
        NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, &global_context.type_bundle, function_type1, NULL,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()), NULL));

    ASSERT_OK(kefir_ast_global_context_define_static_function(&kft_mem, &global_context,
                                                              KEFIR_AST_FUNCTION_SPECIFIER_NONE, type1, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->function.type, type1));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(!scoped_id->function.external);

    struct kefir_ast_function_type *function_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(
        &kft_mem, &global_context.type_bundle,
        kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()), "func1", &function_type2);
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, &global_context.type_bundle, function_type2, NULL,
        kefir_ast_type_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_long_long(),
                             kefir_ast_constant_expression_integer(&kft_mem, 1), NULL),
        NULL));

    ASSERT_NOK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                         type2, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        type2, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_function(&kft_mem, &global_context,
                                                               KEFIR_AST_FUNCTION_SPECIFIER_NONE, type2, NULL, NULL));

    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function_type2, NULL,
                                                kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, &global_context.type_bundle, function_type2, NULL,
        kefir_ast_type_qualified(
            &kft_mem, &global_context.type_bundle,
            kefir_ast_type_pointer(&kft_mem, &global_context.type_bundle, kefir_ast_type_void()),
            (const struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = true}),
        NULL));

    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context,
                                                        KEFIR_AST_FUNCTION_SPECIFIER_NORETURN, type2, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                       type2, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_function(&kft_mem, &global_context,
                                                              KEFIR_AST_FUNCTION_SPECIFIER_NONE, type2, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "func1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->function.type, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->function.type, type2));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NORETURN);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(!scoped_id->function.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_function_scope3, "AST ordinary scope - block scope functions #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "function1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "",
                                                kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "",
                                                kefir_ast_type_double(), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "function1", &function2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, "x", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, "y", NULL, NULL));

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
        ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type1,
                                                           NULL, NULL));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
        ASSERT(scoped_id != NULL);
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
        ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
        ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        ASSERT(scoped_id->function.external);
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
                                                        type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(scoped_id->function.external);

    ASSERT_OK(kefir_ast_global_context_define_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NORETURN,
                                                       type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(!scoped_id->function.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_function_scope4, "AST ordinary scope - block scope functions #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "function1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "",
                                                kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "",
                                                kefir_ast_type_double(), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "function1", &function2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, "x", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, "y", NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_define_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NORETURN,
                                                       type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type2));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NORETURN);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(!scoped_id->function.external);

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
        ASSERT(scoped_id != NULL);
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type2));
        ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NORETURN);
        ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        ASSERT(!scoped_id->function.external);

        ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type1,
                                                           NULL, NULL));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
        ASSERT(scoped_id != NULL);
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
        ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NORETURN);
        ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        ASSERT(!scoped_id->function.external);
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
                                                        type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(!scoped_id->function.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_function_scope5, "AST ordinary scope - block scope functions #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;

    struct kefir_ast_function_type *function1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "function1", &function1);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "",
                                                kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function1, "",
                                                kefir_ast_type_double(), NULL));

    struct kefir_ast_function_type *function2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, &global_context.type_bundle, kefir_ast_type_void(), "function1", &function2);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, "x", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &global_context.type_bundle, function2, "y", NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_define_static_function(&kft_mem, &global_context,
                                                              KEFIR_AST_FUNCTION_SPECIFIER_NONE, type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type2));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(!scoped_id->function.external);

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
        ASSERT(scoped_id != NULL);
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type2));
        ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
        ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
        ASSERT(!scoped_id->function.external);

        ASSERT_OK(kefir_ast_local_context_declare_function(&kft_mem, &context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type1,
                                                           NULL, NULL));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
        ASSERT(scoped_id != NULL);
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
        ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE);
        ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
        ASSERT(!scoped_id->function.external);
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_global_context_declare_function(&kft_mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
                                                        type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "function1", &scoped_id));
    ASSERT(scoped_id != NULL);
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->function.type, type1));
    ASSERT(scoped_id->function.specifier == KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
    ASSERT(scoped_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(!scoped_id->function.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
