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
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"
#include "declarator_analysis.h"

DEFINE_CASE(ast_declarator_analysis1, "AST declarator analysis - declarator type specifiers") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_void(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_void(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_char(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_signed(&kft_mem),
                           kefir_ast_type_specifier_char(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_char(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_short(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_short(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_short(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_signed(&kft_mem),
                           kefir_ast_type_specifier_short(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_short(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_short(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_short(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3, kefir_ast_type_specifier_signed(&kft_mem),
                           kefir_ast_type_specifier_short(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_short(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_short(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_signed(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_signed(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_unsigned(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_unsigned(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_signed(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_signed(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3, kefir_ast_type_specifier_signed(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
                           kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 4,
                           kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_long_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_long_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 4,
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_float(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_float(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_double(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_double(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_long_double(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_specifier_double(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_boolean(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_boolean(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_va_list(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_va_list(&kft_mem));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis2, "AST declarator analysis - declarator type qualifiers") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                    (struct kefir_ast_type_qualification){.constant = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
                           kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_long_long(),
                                                    (struct kefir_ast_type_qualification){.restricted = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_qualifier_restrict(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
                                                    (struct kefir_ast_type_qualification){.volatile_type = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
                           kefir_ast_type_specifier_float(&kft_mem), kefir_ast_type_qualifier_volatile(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
                                                    (struct kefir_ast_type_qualification){.volatile_type = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
                           kefir_ast_type_specifier_float(&kft_mem), kefir_ast_type_qualifier_volatile(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(),
                                 (struct kefir_ast_type_qualification){.constant = true, .restricted = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 5,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_qualifier_restrict(&kft_mem),
        kefir_ast_type_specifier_int(&kft_mem), kefir_ast_type_qualifier_const(&kft_mem),
        kefir_ast_type_specifier_short(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_char(),
                                 (struct kefir_ast_type_qualification){.constant = true, .volatile_type = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 4,
        kefir_ast_type_specifier_char(&kft_mem), kefir_ast_type_specifier_signed(&kft_mem),
        kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_type_qualifier_const(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                 (struct kefir_ast_type_qualification){.restricted = true, .volatile_type = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2,
        kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_type_qualifier_restrict(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(),
                                                    (struct kefir_ast_type_qualification){
                                                        .constant = true, .restricted = true, .volatile_type = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 5,
                           kefir_ast_type_qualifier_restrict(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                           kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_type_specifier_unsigned(&kft_mem),
                           kefir_ast_type_qualifier_volatile(&kft_mem));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis3, "AST declarator analysis - declarator storage class") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 2, kefir_ast_storage_class_specifier_typedef(&kft_mem),
                           kefir_ast_type_specifier_char(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_short(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 4,
                           kefir_ast_type_specifier_int(&kft_mem), kefir_ast_storage_class_specifier_extern(&kft_mem),
                           kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_double(),
                                                    (struct kefir_ast_type_qualification){.constant = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_specifier_double(&kft_mem),
                           kefir_ast_storage_class_specifier_static(&kft_mem),
                           kefir_ast_type_qualifier_const(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_long_long(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_storage_class_specifier_thread_local(&kft_mem),
                           kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_int(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0,
                           2, kefir_ast_storage_class_specifier_extern(&kft_mem),
                           kefir_ast_storage_class_specifier_thread_local(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3, kefir_ast_storage_class_specifier_thread_local(&kft_mem),
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_storage_class_specifier_extern(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, kefir_ast_type_signed_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 4, kefir_ast_storage_class_specifier_static(&kft_mem),
        kefir_ast_type_specifier_char(&kft_mem), kefir_ast_storage_class_specifier_thread_local(&kft_mem),
        kefir_ast_type_specifier_signed(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
                                 (struct kefir_ast_type_qualification){.volatile_type = true, .restricted = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 5,
        kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_storage_class_specifier_thread_local(&kft_mem),
        kefir_ast_type_specifier_float(&kft_mem), kefir_ast_type_qualifier_restrict(&kft_mem),
        kefir_ast_storage_class_specifier_static(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_short(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 4, kefir_ast_type_specifier_unsigned(&kft_mem),
                           kefir_ast_type_specifier_int(&kft_mem), kefir_ast_storage_class_specifier_auto(&kft_mem),
                           kefir_ast_type_specifier_short(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_int(),
                                                    (struct kefir_ast_type_qualification){
                                                        .constant = true,
                                                    }),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 3,
                           kefir_ast_type_qualifier_const(&kft_mem),
                           kefir_ast_storage_class_specifier_register(&kft_mem),
                           kefir_ast_type_specifier_unsigned(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_signed_int(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_signed(&kft_mem));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis4, "AST declarator analysis - function declarator specifier") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *func_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_char(), &func_type1);

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, context->type_bundle,
                                kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                         (struct kefir_ast_type_qualification){.constant = true}),
                                &func_type2);

    struct kefir_ast_function_type *func_type3 = NULL;
    const struct kefir_ast_type *type3 =
        kefir_ast_type_function(&kft_mem, context->type_bundle,
                                kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
                                                         (struct kefir_ast_type_qualification){.volatile_type = true}),
                                &func_type3);

    struct kefir_ast_function_type *func_type4 = NULL;
    const struct kefir_ast_type *type4 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(), &func_type4);

    struct kefir_ast_function_type *func_type5 = NULL;
    const struct kefir_ast_type *type5 = kefir_ast_type_function(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long_long(),
                                 (struct kefir_ast_type_qualification){.restricted = true, .constant = true}),
        &func_type5);

    ASSERT_FUNCTION_TYPE(&kft_mem, context, type1, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                         KEFIR_AST_FUNCTION_SPECIFIER_NONE, 1, kefir_ast_type_specifier_char(&kft_mem));

    ASSERT_FUNCTION_TYPE(&kft_mem, context, type1, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                         KEFIR_AST_FUNCTION_SPECIFIER_INLINE, 2, kefir_ast_type_specifier_char(&kft_mem),
                         kefir_ast_function_specifier_inline(&kft_mem));

    ASSERT_FUNCTION_TYPE(&kft_mem, context, type1, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                         KEFIR_AST_FUNCTION_SPECIFIER_INLINE, 4, kefir_ast_function_specifier_inline(&kft_mem),
                         kefir_ast_type_specifier_char(&kft_mem), kefir_ast_storage_class_specifier_static(&kft_mem),
                         kefir_ast_function_specifier_inline(&kft_mem));

    ASSERT_FUNCTION_TYPE(&kft_mem, context, type2, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                         KEFIR_AST_FUNCTION_SPECIFIER_NORETURN, 3, kefir_ast_type_specifier_int(&kft_mem),
                         kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_function_specifier_noreturn(&kft_mem));

    ASSERT_FUNCTION_TYPE(&kft_mem, context, type3, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                         KEFIR_AST_FUNCTION_SPECIFIER_NORETURN, 5, kefir_ast_storage_class_specifier_extern(&kft_mem),
                         kefir_ast_function_specifier_noreturn(&kft_mem), kefir_ast_type_specifier_float(&kft_mem),
                         kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_function_specifier_noreturn(&kft_mem));

    ASSERT_FUNCTION_TYPE(&kft_mem, context, type4, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                         KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN, 4, kefir_ast_type_specifier_short(&kft_mem),
                         kefir_ast_function_specifier_noreturn(&kft_mem), kefir_ast_type_specifier_unsigned(&kft_mem),
                         kefir_ast_function_specifier_inline(&kft_mem));

    ASSERT_FUNCTION_TYPE(&kft_mem, context, type5, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                         KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN, 11, kefir_ast_type_specifier_int(&kft_mem),
                         kefir_ast_function_specifier_inline(&kft_mem), kefir_ast_type_specifier_unsigned(&kft_mem),
                         kefir_ast_storage_class_specifier_extern(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                         kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_function_specifier_noreturn(&kft_mem),
                         kefir_ast_type_qualifier_restrict(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
                         kefir_ast_function_specifier_noreturn(&kft_mem),
                         kefir_ast_function_specifier_inline(&kft_mem));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis5, "AST declarator analysis - declarator alignment #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_char(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, 1, 2,
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))),
        kefir_ast_type_specifier_char(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, 2, 3,
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))),
        kefir_ast_type_specifier_char(&kft_mem), kefir_ast_storage_class_specifier_auto(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, kefir_ast_type_unsigned_short(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, 4, 6, kefir_ast_type_specifier_unsigned(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))),
        kefir_ast_storage_class_specifier_extern(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4))),
        kefir_ast_type_specifier_short(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2))));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                 (struct kefir_ast_type_qualification){.volatile_type = true}),
        KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 4, 6,
        kefir_ast_type_qualifier_volatile(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4))),
        kefir_ast_storage_class_specifier_thread_local(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4))),
        kefir_ast_storage_class_specifier_static(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, kefir_ast_type_unsigned_long_long(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, 8, 8, kefir_ast_type_specifier_unsigned(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))),
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_storage_class_specifier_register(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))),
        kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4))),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 8))));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_declarator_analysis6, "AST declarator analysis - declarator alignment #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

#define MAKE_TYPENAME(_id, _spec_count, ...)                                                      \
    struct kefir_ast_type_name *_id =                                                             \
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)); \
    ASSERT_OK(append_specifiers(&kft_mem, &_id->type_decl.specifiers, (_spec_count), __VA_ARGS__));

    MAKE_TYPENAME(type_name1, 1, kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name2, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem));
    MAKE_TYPENAME(type_name3, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name4, 1, kefir_ast_type_specifier_float(&kft_mem));
    MAKE_TYPENAME(type_name5, 1, kefir_ast_type_specifier_short(&kft_mem));
    MAKE_TYPENAME(type_name6, 1, kefir_ast_type_specifier_int(&kft_mem));
    MAKE_TYPENAME(type_name7, 2, kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    MAKE_TYPENAME(type_name8, 1, kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name9, 2, kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    MAKE_TYPENAME(type_name10, 1, kefir_ast_type_specifier_double(&kft_mem));
#undef MAKE_TYPENAME

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 0, 1, kefir_ast_type_specifier_char(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 1, 2,
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)),
                           kefir_ast_type_specifier_char(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_char(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                           KEFIR_AST_FUNCTION_SPECIFIER_NONE, 2, 3,
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name2)),
                           kefir_ast_type_specifier_char(&kft_mem), kefir_ast_storage_class_specifier_auto(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context, kefir_ast_type_unsigned_short(),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 4, 6,
                           kefir_ast_type_specifier_unsigned(&kft_mem),
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name3)),
                           kefir_ast_storage_class_specifier_extern(&kft_mem),
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name4)),
                           kefir_ast_type_specifier_short(&kft_mem),
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name5)));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
                           kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                                    (struct kefir_ast_type_qualification){.volatile_type = true}),
                           KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, KEFIR_AST_FUNCTION_SPECIFIER_NONE, 4,
                           6, kefir_ast_type_qualifier_volatile(&kft_mem),
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name6)),
                           kefir_ast_storage_class_specifier_thread_local(&kft_mem),
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name7)),
                           kefir_ast_storage_class_specifier_static(&kft_mem),
                           kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name8)));

    ASSERT_IDENTIFIER_TYPE(
        &kft_mem, context, kefir_ast_type_unsigned_long_long(), KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
        KEFIR_AST_FUNCTION_SPECIFIER_NONE, 8, 8, kefir_ast_type_specifier_unsigned(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1))),
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_storage_class_specifier_register(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name9)),
        kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 4))),
        kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(type_name10)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
