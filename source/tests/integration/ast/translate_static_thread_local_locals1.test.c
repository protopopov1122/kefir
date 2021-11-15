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
#include "kefir/ast-translator/function_definition.h"
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
        kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));

    struct kefir_ast_translator_context global_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&global_translator_context, &global_context.context, &env, &module));
    global_translator_context.global_scope_layout = &translator_global_scope;

    struct kefir_ast_declarator *function1_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "fn"));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "value"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_static(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                          kefir_ast_storage_class_specifier_thread_local(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_int(mem)));

    struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
        mem, kefir_ast_declarator_identifier(mem, global_context.context.symbols, "increment"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_int(mem)));

    struct kefir_ast_compound_statement *function1_body = kefir_ast_new_compound_statement(mem);
    REQUIRE_OK(kefir_list_insert_after(mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                                       KEFIR_AST_NODE_BASE(decl1)));
    REQUIRE_OK(kefir_list_insert_after(mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                                       KEFIR_AST_NODE_BASE(decl2)));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
            mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(
                mem, KEFIR_AST_ASSIGNMENT_ADD,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, global_context.context.symbols, "value")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, global_context.context.symbols, "increment"))))))));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
        KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, global_context.context.symbols, "value"))))));

    struct kefir_ast_function_definition *function1 =
        kefir_ast_new_function_definition(mem, function1_decl, function1_body);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &function1->specifiers, kefir_ast_type_specifier_int(mem)));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(function1)));

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));

    struct kefir_ast_translator_function_context func_ctx;
    REQUIRE_OK(kefir_ast_translator_function_context_init(mem, &global_translator_context, function1, &func_ctx));
    REQUIRE_OK(kefir_ast_translator_function_context_translate(mem, &func_ctx));
    REQUIRE_OK(kefir_ast_translator_function_context_finalize(mem, &func_ctx));
    REQUIRE_OK(kefir_ast_translator_function_context_free(mem, &func_ctx));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(function1)));

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
