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

DEFINE_CASE(ast_ordinary_typedef_scope1, "AST ordinary scope - type definitions #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    ASSERT_OK(
        kefir_ast_global_context_define_type(&kft_mem, &global_context, "type1", kefir_ast_type_void(), NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type2", kefir_ast_type_signed_long(),
                                                   NULL, NULL));
    ASSERT_OK(
        kefir_ast_global_context_define_type(&kft_mem, &global_context, "type3", kefir_ast_type_float(), NULL, NULL));

    ASSERT_NOK(kefir_ast_global_context_define_type(
        &kft_mem, &global_context, "type1", kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()), NULL,
        NULL));
    ASSERT_OK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "type2", kefir_ast_type_signed_long(),
                                                   NULL, NULL));
    ASSERT_NOK(
        kefir_ast_global_context_define_type(&kft_mem, &global_context, "type3", kefir_ast_type_double(), NULL, NULL));

    ASSERT_OK(kefir_ast_global_context_define_static(&kft_mem, &global_context, "val1", kefir_ast_type_unsigned_char(),
                                                     NULL, NULL, NULL, NULL));
    ASSERT_NOK(kefir_ast_global_context_define_type(&kft_mem, &global_context, "val1", kefir_ast_type_unsigned_char(),
                                                    NULL, NULL));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_void()));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_signed_long()));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_float()));

    ASSERT_NOK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type4", &scoped_id));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "val1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));

        ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &context, "type1", kefir_ast_type_unsigned_char(),
                                                        NULL, NULL, NULL, NULL));

        ASSERT_OK(kefir_ast_local_context_define_type(
            &kft_mem, &context, "val1",
            kefir_ast_type_unbounded_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long(), NULL), NULL,
            NULL));

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "val1", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
        ASSERT(KEFIR_AST_TYPE_SAME(
            scoped_id->type,
            kefir_ast_type_unbounded_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long(), NULL)));

        do {
            ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));

            ASSERT_OK(kefir_ast_local_context_define_type(
                &kft_mem, &context, "type2",
                kefir_ast_type_qualified(&kft_mem, &type_bundle, kefir_ast_type_bool(),
                                         (const struct kefir_ast_type_qualification){
                                             .constant = false, .restricted = true, .volatile_type = false}),
                NULL, NULL));

            ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
            ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
            ASSERT(KEFIR_AST_TYPE_SAME(
                scoped_id->type,
                kefir_ast_type_qualified(&kft_mem, &type_bundle, kefir_ast_type_bool(),
                                         (const struct kefir_ast_type_qualification){
                                             .constant = false, .restricted = true, .volatile_type = false})));

            ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
        } while (0);

        ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
        ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_signed_long()));

        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_void()));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_signed_long()));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION);
    ASSERT(KEFIR_AST_TYPE_SAME(scoped_id->type, kefir_ast_type_float()));

    ASSERT_NOK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "type4", &scoped_id));

    ASSERT_OK(kefir_ast_local_context_resolve_scoped_ordinary_identifier(&context, "val1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
