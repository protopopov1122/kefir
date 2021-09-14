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

#define ASSERT_RESOLVED_IDENTIFIER(_context, _identifier, _storage, _type, _linkage)                              \
    do {                                                                                                          \
        const struct kefir_ast_scoped_identifier *__scoped_id = NULL;                                             \
        ASSERT_OK(                                                                                                \
            kefir_ast_local_context_resolve_scoped_ordinary_identifier((_context), (_identifier), &__scoped_id)); \
        ASSERT(__scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);                                          \
        ASSERT(__scoped_id->object.storage == (_storage));                                                        \
        ASSERT(KEFIR_AST_TYPE_COMPATIBLE((_context)->global->type_traits, __scoped_id->object.type, (_type)));    \
        ASSERT(__scoped_id->object.linkage == (_linkage));                                                        \
    } while (0)

DEFINE_CASE(ast_ordinary_scope_objects1, "AST Declaration scoping - global rules for extern") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                         kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_long(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "ext_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects2, "AST Declaration scoping - global rules for thread_local extern") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                      kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                         kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "ext_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects3, "AST Declaration scoping - global rules for extern #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1",
                                                       kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1",
                                                       kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                         kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_long(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "ext_int1",
                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1",
                                                       kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "ext_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects4, "AST Declaration scoping - global rules for thread_local extern #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                      kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_long(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                         kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "ext_int1",
                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "ext_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects5, "AST Declaration scoping - global rules for static") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1",
                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1",
                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1",
                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1",
                                                      kefir_ast_type_signed_long(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1",
                                                         kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1",
                                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(
        &kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1",
                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "static_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int2",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int2",
                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int3",
                                                       kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int3",
                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects6, "AST Declaration scoping - global rules for thread_local static") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(
        &kft_mem, &global_context, "static_int1", kefir_ast_type_signed_long(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1",
                                                                      kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "static_int1",
                                                         kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "static_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "static_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "static_int1",
                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "static_int2",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(
        &kft_mem, &global_context, "static_int2", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "static_int3", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_static_thread_local(
        &kft_mem, &global_context, "static_int3", kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "static_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects7, "AST Declaration scoping - local extern") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL,
                                                     NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "ext_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_long(),
                                                        NULL, NULL, NULL));
    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
        ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                           NULL, NULL, NULL));
        ASSERT_NOK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1",
                                                            kefir_ast_type_signed_long(), NULL, NULL, NULL));
        do {
            ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
            ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                            NULL, NULL, NULL, NULL));
            do {
                ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
                ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1",
                                                                   kefir_ast_type_signed_int(), NULL, NULL, NULL));
                ASSERT_NOK(kefir_ast_local_context_declare_external_thread_local(
                    &kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL, NULL, NULL));
                ASSERT_NOK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1",
                                                                    kefir_ast_type_signed_long(), NULL, NULL, NULL));
                ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
            } while (0);
            ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
        } while (0);
        ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                           NULL, NULL, NULL));
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                      kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                         kefir_ast_type_signed_long(), NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "ext_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects8, "AST Declaration scoping - local extern thread_local") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(), NULL,
                                                     NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "ext_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                        NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                     kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                     kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "ext_int1",
                                                                 kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external(&kft_mem, &context, "ext_int1", kefir_ast_type_signed_int(),
                                                        NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                     kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "ext_int1",
                                                                    kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "ext_int1",
                                                         kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "ext_int1",
                                                                      kefir_ast_type_signed_long(), NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "ext_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects9, "AST Declaration scoping - local static") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                    NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                     NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                        NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                    NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                     NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                 kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                     NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                     NULL, NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "static_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects10, "AST Declaration scoping - local static thread_local") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                 kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_declare_external_thread_local(&kft_mem, &context, "static_int1",
                                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                 kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                    NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "static_int1", kefir_ast_type_signed_int(),
                                                     NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "static_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "static_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects11, "AST Declaration scoping - local auto") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL,
                                                  NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL,
                                                   NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL,
                                                     NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "auto_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL,
                                                  NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL,
                                                  NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL,
                                                   NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL,
                                                  NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int3", kefir_ast_type_signed_int(),
                                                      NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL,
                                                    NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "auto_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects12, "AST Declaration scoping - local register") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(),
                                                      NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL,
                                                     NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_static_thread_local(&kft_mem, &context, "auto_int1",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(),
                                                      NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(),
                                                      NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(),
                                                      NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int2", kefir_ast_type_signed_int(), NULL,
                                                   NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context, "auto_int3", kefir_ast_type_signed_int(), NULL,
                                                  NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "auto_int1", kefir_ast_type_signed_int(), NULL,
                                                    NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "auto_int1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, scoped_id->object.type, kefir_ast_type_signed_int()));
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT(!scoped_id->object.external);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects13, "AST Declaration scoping - block scoping") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "variable1",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external_thread_local(&kft_mem, &global_context, "variable2",
                                                                     kefir_ast_type_signed_long(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "variable1",
                                                       kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "variable3", kefir_ast_type_bool(),
                                                       NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "variable4",
                                                     kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "variable4",
                                                        kefir_ast_type_signed_int(), NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_static_thread_local(&kft_mem, &global_context, "variable5",
                                                                  kefir_ast_type_signed_int(), NULL, NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable3", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_bool(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable4", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable5", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_OK(kefir_ast_local_context_declare_external(&kft_mem, &context, "variable1", kefir_ast_type_signed_int(),
                                                       NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "variable2", kefir_ast_type_float(), NULL, NULL,
                                                    NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, kefir_ast_type_float(),
                               KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &context, "variable10", kefir_ast_type_unsigned_int(), NULL,
                                                  NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable10", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                               kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
    ASSERT_OK(kefir_ast_local_context_define_register(&kft_mem, &context, "variable10", kefir_ast_type_unsigned_int(),
                                                      NULL, NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable10", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                               kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_local_context_define_static_thread_local(
        &kft_mem, &context, "variable2", kefir_ast_type_unsigned_int(), NULL, NULL, NULL, NULL));
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));

    ASSERT_RESOLVED_IDENTIFIER(&context, "variable10", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
                               kefir_ast_type_unsigned_int(), KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE);
    ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));

    ASSERT_RESOLVED_IDENTIFIER(&context, "variable1", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable2", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
                               kefir_ast_type_signed_long(), KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable3", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, kefir_ast_type_bool(),
                               KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable4", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);
    ASSERT_RESOLVED_IDENTIFIER(&context, "variable5", KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                               kefir_ast_type_signed_int(), KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects14, "AST Declaration scoping - block externals") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *array_type1 =
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(), NULL);
    const struct kefir_ast_type *array_type2 =
        kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                                    kefir_ast_constant_expression_integer(&kft_mem, 100), NULL);
    const struct kefir_ast_type *array_type3 =
        kefir_ast_type_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                             kefir_ast_constant_expression_integer(&kft_mem, 100), NULL);

    ASSERT_OK(
        kefir_ast_global_context_declare_external(&kft_mem, &global_context, "array1", array_type1, NULL, NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(scoped_id->object.external);
    ASSERT(KEFIR_AST_TYPE_SAME(array_type1, scoped_id->object.type));
    ASSERT(scoped_id->object.alignment->value == 0);

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
        ASSERT_OK(kefir_ast_local_context_declare_external(
            &kft_mem, &context, "array1", array_type2,
            kefir_ast_alignment_const_expression(&kft_mem, kefir_ast_constant_expression_integer(&kft_mem, 16)), NULL,
            NULL));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
        ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
        ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
        ASSERT(scoped_id->object.external);
        ASSERT(KEFIR_AST_TYPE_SAME(array_type3, scoped_id->object.type));
        ASSERT(scoped_id->object.alignment->value == 16);
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(scoped_id->object.external);
    ASSERT(KEFIR_AST_TYPE_SAME(array_type3, scoped_id->object.type));
    ASSERT(scoped_id->object.alignment->value == 16);

    ASSERT_OK(kefir_ast_global_context_define_external(&kft_mem, &global_context, "array1", array_type3, NULL, NULL,
                                                       NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN);
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(!scoped_id->object.external);
    ASSERT(KEFIR_AST_TYPE_SAME(array_type3, scoped_id->object.type));
    ASSERT(scoped_id->object.alignment->value == 16);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_ordinary_scope_objects15, "AST Declaration scoping - block externals #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_type *array_type1 =
        kefir_ast_type_unbounded_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(), NULL);
    const struct kefir_ast_type *array_type2 =
        kefir_ast_type_array_static(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                                    kefir_ast_constant_expression_integer(&kft_mem, 100), NULL);
    const struct kefir_ast_type *array_type3 =
        kefir_ast_type_array(&kft_mem, &global_context.type_bundle, kefir_ast_type_signed_int(),
                             kefir_ast_constant_expression_integer(&kft_mem, 100), NULL);

    ASSERT_OK(kefir_ast_global_context_define_external_thread_local(
        &kft_mem, &global_context, "array1", array_type1,
        kefir_ast_alignment_const_expression(&kft_mem, kefir_ast_constant_expression_integer(&kft_mem, 8)), NULL, NULL,
        NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(!scoped_id->object.external);
    ASSERT(KEFIR_AST_TYPE_SAME(array_type1, scoped_id->object.type));
    ASSERT(scoped_id->object.alignment->value == 8);

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
        ASSERT_OK(kefir_ast_local_context_declare_external_thread_local(
            &kft_mem, &context, "array1", array_type2,
            kefir_ast_alignment_const_expression(&kft_mem, kefir_ast_constant_expression_integer(&kft_mem, 2)), NULL,
            NULL));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
        ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
        ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
        ASSERT(!scoped_id->object.external);
        ASSERT(KEFIR_AST_TYPE_SAME(array_type3, scoped_id->object.type));
        ASSERT(scoped_id->object.alignment->value == 8);
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "array1", &scoped_id));
    ASSERT(scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL);
    ASSERT(scoped_id->object.linkage == KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE);
    ASSERT(!scoped_id->object.external);
    ASSERT(KEFIR_AST_TYPE_SAME(array_type3, scoped_id->object.type));
    ASSERT(scoped_id->object.alignment->value == 8);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
