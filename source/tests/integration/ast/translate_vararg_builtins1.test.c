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

#include "kefir/core/mem.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/test/util.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/format.h"
#include "./expression.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_util_default_type_traits(), &env.target_env, &global_context, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));

    struct kefir_ast_translator_context global_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &global_translator_context, &global_context.context, &env,
                                                 &module, NULL));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC2("vararg", {
        struct kefir_ast_function_type *function_type = NULL;
        const struct kefir_ast_type *type1 =
            kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_void(), &function_type);
        function_type->ellipsis = true;
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_OK(global_context.context.define_identifier(
            mem, &global_context.context, true, "fn0", type1, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, NULL, &scoped_id));
        local_context.context.surrounding_function = scoped_id;

        struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
            mem, kefir_ast_declarator_identifier(mem, context->symbols, "vararg"), NULL, NULL);
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_va_list(mem)));

        struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
            mem, kefir_ast_declarator_identifier(mem, context->symbols, "vararg2"), NULL, NULL);
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_va_list(mem)));

        struct kefir_ast_declaration *decl3 = kefir_ast_new_single_declaration(
            mem, kefir_ast_declarator_identifier(mem, context->symbols, "x"), NULL, NULL);
        REQUIRE_OK(
            kefir_ast_declarator_specifier_list_append(mem, &decl3->specifiers, kefir_ast_type_specifier_float(mem)));

        struct kefir_ast_builtin *builtin1 = kefir_ast_new_builtin(mem, KEFIR_AST_BUILTIN_VA_START);
        REQUIRE_OK(kefir_ast_builtin_append(
            mem, builtin1, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "vararg"))));
        REQUIRE_OK(kefir_ast_builtin_append(mem, builtin1,
                                            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "x"))));
        struct kefir_ast_expression_statement *vararg_start =
            kefir_ast_new_expression_statement(mem, KEFIR_AST_NODE_BASE(builtin1));

        struct kefir_ast_builtin *builtin2 = kefir_ast_new_builtin(mem, KEFIR_AST_BUILTIN_VA_COPY);
        REQUIRE_OK(kefir_ast_builtin_append(
            mem, builtin2, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "vararg2"))));
        REQUIRE_OK(kefir_ast_builtin_append(
            mem, builtin2, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "vararg"))));
        struct kefir_ast_expression_statement *vararg_copy =
            kefir_ast_new_expression_statement(mem, KEFIR_AST_NODE_BASE(builtin2));

        struct kefir_ast_type_name *type_name1 =
            kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                                              kefir_ast_type_specifier_double(mem)));
        struct kefir_ast_builtin *builtin3 = kefir_ast_new_builtin(mem, KEFIR_AST_BUILTIN_VA_ARG);
        REQUIRE_OK(kefir_ast_builtin_append(
            mem, builtin3, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "vararg2"))));
        REQUIRE_OK(kefir_ast_builtin_append(mem, builtin3, KEFIR_AST_NODE_BASE(type_name1)));
        struct kefir_ast_expression_statement *vararg_arg =
            kefir_ast_new_expression_statement(mem, KEFIR_AST_NODE_BASE(builtin3));

        struct kefir_ast_builtin *builtin4 = kefir_ast_new_builtin(mem, KEFIR_AST_BUILTIN_VA_END);
        REQUIRE_OK(kefir_ast_builtin_append(
            mem, builtin4, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "vararg"))));
        struct kefir_ast_expression_statement *vararg_end =
            kefir_ast_new_expression_statement(mem, KEFIR_AST_NODE_BASE(builtin4));

        struct kefir_ast_expression_statement *assign1 = kefir_ast_new_expression_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_simple_assignment(
                     mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "vararg")),
                     KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "vararg2")))));

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(decl1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(decl2)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(decl3)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(vararg_start)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(vararg_copy)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(vararg_arg)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(vararg_end)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(assign1)));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
