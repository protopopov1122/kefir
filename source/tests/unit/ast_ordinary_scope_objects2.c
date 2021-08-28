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
#include "kefir/ast/node.h"
#include "kefir/ast/type.h"
#include "kefir/test/util.h"

#define ASSERT_RESOLVED_IDENTIFIER(_context, _identifier, _storage, _type, _linkage)                              \
    do {                                                                                                          \
        const struct kefir_ast_scoped_identifier *__scoped_id = NULL;                                             \
        ASSERT_OK(                                                                                                \
            kefir_ast_local_context_resolve_scoped_ordinary_identifier((_context), (_identifier), &__scoped_id)); \
        ASSERT(__scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);                                          \
        ASSERT(__scoped_id->object.storage == (_storage));                                                        \
        ASSERT(KEFIR_AST_TYPE_SAME(__scoped_id->object.type, (_type)));                                           \
        ASSERT(__scoped_id->object.linkage == (_linkage));                                                        \
    } while (0)

DEFINE_CASE(ast_ordinary_scope_objects_init1, "AST Declaration initializers - global externals #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var1", kefir_ast_type_signed_int(),
                                                        NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_signed_int(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_signed_int(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 101)));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", kefir_ast_type_signed_int(),
                                                       NULL, init1, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_signed_int(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init2 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 101)));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", kefir_ast_type_signed_int(),
                                                        NULL, init2, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_signed_int(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_signed_int(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var1", kefir_ast_type_signed_int(),
                                                        NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_signed_int(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_init2, "AST Declaration initializers - global externals #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(), NULL);
    const struct kefir_ast_type *type2 =
        kefir_ast_type_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 14), NULL);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var1", type1, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, type1,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", type1, NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, type1,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init0 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", type1, NULL, init0, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, type1,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init1 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", type1, NULL, init1, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", type1, NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "var1", type1, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello!")));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "var1", type1, NULL, init2, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init0));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_init3, "AST Declaration initializers - global static #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", kefir_ast_type_signed_long(),
                                                     NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", kefir_ast_type_signed_long(),
                                                     NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    struct kefir_ast_initializer *init0 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 10)));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", kefir_ast_type_signed_long(),
                                                     NULL, init0, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 15)));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", kefir_ast_type_signed_long(),
                                                      NULL, init1, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", kefir_ast_type_signed_long(),
                                                     NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init0));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_init4, "AST Declaration initializers - global static #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(), NULL);
    const struct kefir_ast_type *type2 =
        kefir_ast_type_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 14), NULL);

    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", type1, NULL, NULL, NULL));

    struct kefir_ast_initializer *init1 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", type1, NULL, init1, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", type1, NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello")));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "var1", type1, NULL, init2, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_init5, "AST Declaration initializers - global thread local externals #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "var1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 101)));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1",
                                                                    kefir_ast_type_signed_int(), NULL, init1, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init2 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 101)));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1",
                                                                     kefir_ast_type_signed_int(), NULL, init2, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "var1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_init6, "AST Declaration initializers - global thread local externals #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(), NULL);
    const struct kefir_ast_type *type2 =
        kefir_ast_type_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 14), NULL);

    ASSERT_OK(
        kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "var1", type1, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, type1,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1", type1, NULL,
                                                                    NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, type1,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init0 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1", type1, NULL,
                                                                     init0, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init0));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, type1,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init1 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1", type1, NULL,
                                                                    init1, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1", type1, NULL,
                                                                    NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(
        kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "var1", type1, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello!")));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(&kft_mem, &global_context, "var1", type1, NULL,
                                                                     init2, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_init7, "AST Declaration initializers - global thread local static #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1",
                                                                  kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1",
                                                                  kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    struct kefir_ast_initializer *init0 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 10)));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1",
                                                                  kefir_ast_type_signed_long(), NULL, init0, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    struct kefir_ast_initializer *init1 =
        kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 15)));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1",
                                                                   kefir_ast_type_signed_long(), NULL, init1, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1",
                                                                  kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init0));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects_init8, "AST Declaration initializers - global thread local static #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *type1 =
        kefir_ast_type_unbounded_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(), NULL);
    const struct kefir_ast_type *type2 =
        kefir_ast_type_array(&kft_mem, context.context.type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 14), NULL);

    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1", type1, NULL, NULL,
                                                                   NULL));

    struct kefir_ast_initializer *init1 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello, world!")));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1", type1, NULL, init1,
                                                                  NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1", type1, NULL, NULL,
                                                                  NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    struct kefir_ast_initializer *init2 = kefir_ast_new_expression_initializer(
        &kft_mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(&kft_mem, "Hello")));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "var1", type1, NULL,
                                                                   init2, NULL));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_RESOLVED_IDENTIFIER(&context, "var1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, type2,
                               KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
}
END_CASE
