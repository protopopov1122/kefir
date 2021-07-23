/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

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
#include "kefir/ast/local_context.h"
#include "kefir/ast/function_declaration_context.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_label_scope1, "AST ordinary scope - label scope #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_NOK(global_context.context.reference_label(&kft_mem, &global_context.context, "label1", false, NULL));
    ASSERT_NOK(global_context.context.reference_label(&kft_mem, &global_context.context, "label1", true, NULL));
    ASSERT_NOK(global_context.context.reference_label(&kft_mem, &global_context.context, "label1", false, &scoped_id));
    ASSERT_NOK(global_context.context.reference_label(&kft_mem, &global_context.context, "label1", true, &scoped_id));

    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_label_scope2, "AST ordinary scope - label scope #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_function_declaration_context func_decl_context;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_function_declaration_context_init(&kft_mem, &global_context.context, &func_decl_context));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT_NOK(func_decl_context.context.reference_label(&kft_mem, &func_decl_context.context, "label1", false, NULL));
    ASSERT_NOK(func_decl_context.context.reference_label(&kft_mem, &func_decl_context.context, "label1", true, NULL));
    ASSERT_NOK(
        func_decl_context.context.reference_label(&kft_mem, &func_decl_context.context, "label1", false, &scoped_id));
    ASSERT_NOK(
        func_decl_context.context.reference_label(&kft_mem, &func_decl_context.context, "label1", true, &scoped_id));

    ASSERT_OK(kefir_ast_function_declaration_context_free(&kft_mem, &func_decl_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_label_scope3, "AST ordinary scope - label scope #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;

#define ASSERT_LABEL(_mem, _context, _label, _define, _defined)                                      \
    do {                                                                                             \
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;                                  \
        ASSERT_OK((_context)->reference_label((_mem), (_context), (_label), (_define), &scoped_id)); \
        ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);                                \
        ASSERT(scoped_id->label.defined == (_defined));                                              \
        ASSERT(scoped_id->label.point != NULL);                                                      \
    } while (0)

#define ASSERT_LABEL_NOK(_mem, _context, _label, _define)                                       \
    do {                                                                                        \
        ASSERT_NOK((_context)->reference_label((_mem), (_context), (_label), (_define), NULL)); \
    } while (0)

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT(context.context.resolve_label_identifier(&context.context, "label1", &scoped_id) == KEFIR_NOT_FOUND);
    ASSERT(context.context.resolve_label_identifier(&context.context, "label2", &scoped_id) == KEFIR_NOT_FOUND);
    ASSERT(context.context.resolve_label_identifier(&context.context, "label3", &scoped_id) == KEFIR_NOT_FOUND);

    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, false);
    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, false);
    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, false);
    ASSERT_LABEL(&kft_mem, &context.context, "label1", true, true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label1", true);
    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label1", true);
    ASSERT_LABEL(&kft_mem, &context.context, "label2", true, true);
    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label1", true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label2", true);
    ASSERT_LABEL(&kft_mem, &context.context, "label2", false, true);
    ASSERT_LABEL(&kft_mem, &context.context, "label3", false, false);
    ASSERT_LABEL(&kft_mem, &context.context, "label3", true, true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label1", true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label2", true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label3", true);
    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, true);
    ASSERT_LABEL(&kft_mem, &context.context, "label2", false, true);
    ASSERT_LABEL(&kft_mem, &context.context, "label3", false, true);

    ASSERT_OK(context.context.resolve_label_identifier(&context.context, "label1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
    ASSERT(scoped_id->label.point != NULL);

    ASSERT_OK(context.context.resolve_label_identifier(&context.context, "label2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
    ASSERT(scoped_id->label.point != NULL);

    ASSERT_OK(context.context.resolve_label_identifier(&context.context, "label3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
    ASSERT(scoped_id->label.point != NULL);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_label_scope4, "AST ordinary scope - label scope #4") {
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

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    ASSERT(context.context.resolve_label_identifier(&context.context, "label1", &scoped_id) == KEFIR_NOT_FOUND);
    ASSERT(context.context.resolve_label_identifier(&context.context, "label2", &scoped_id) == KEFIR_NOT_FOUND);
    ASSERT(context.context.resolve_label_identifier(&context.context, "label3", &scoped_id) == KEFIR_NOT_FOUND);
    ASSERT(context.context.resolve_label_identifier(&context.context, "label4", &scoped_id) == KEFIR_NOT_FOUND);

    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, false);

    do {
        ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
        ASSERT_LABEL(&kft_mem, &context.context, "label1", false, false);
        ASSERT_LABEL(&kft_mem, &context.context, "label1", true, true);
        ASSERT_LABEL(&kft_mem, &context.context, "label2", true, true);

        do {
            ASSERT_OK(kefir_ast_local_context_push_block_scope(&kft_mem, &context));
            ASSERT_LABEL(&kft_mem, &context.context, "label1", false, true);
            ASSERT_LABEL(&kft_mem, &context.context, "label2", false, true);
            ASSERT_LABEL(&kft_mem, &context.context, "label3", false, false);
            ASSERT_LABEL(&kft_mem, &context.context, "label4", false, false);
            ASSERT_LABEL_NOK(&kft_mem, &context.context, "label2", true);
            ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
        } while (0);

        ASSERT_LABEL(&kft_mem, &context.context, "label3", false, false);
        ASSERT_LABEL(&kft_mem, &context.context, "label4", false, false);
        ASSERT_LABEL(&kft_mem, &context.context, "label4", true, true);
        ASSERT_OK(kefir_ast_local_context_pop_block_scope(&context));
    } while (0);

    ASSERT_LABEL(&kft_mem, &context.context, "label1", false, true);
    ASSERT_LABEL(&kft_mem, &context.context, "label2", false, true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label2", true);
    ASSERT_LABEL(&kft_mem, &context.context, "label3", false, false);
    ASSERT_LABEL(&kft_mem, &context.context, "label4", false, true);
    ASSERT_LABEL_NOK(&kft_mem, &context.context, "label4", true);

    ASSERT_OK(context.context.resolve_label_identifier(&context.context, "label1", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
    ASSERT(scoped_id->label.defined);
    ASSERT(scoped_id->label.point != NULL);

    ASSERT_OK(context.context.resolve_label_identifier(&context.context, "label2", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
    ASSERT(scoped_id->label.defined);
    ASSERT(scoped_id->label.point != NULL);

    ASSERT_OK(context.context.resolve_label_identifier(&context.context, "label3", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
    ASSERT(!scoped_id->label.defined);
    ASSERT(scoped_id->label.point != NULL);

    ASSERT_OK(context.context.resolve_label_identifier(&context.context, "label4", &scoped_id));
    ASSERT(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL);
    ASSERT(scoped_id->label.defined);
    ASSERT(scoped_id->label.point != NULL);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
