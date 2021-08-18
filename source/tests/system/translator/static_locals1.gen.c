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

#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"

static struct kefir_ast_function_definition *define_sum_function(struct kefir_mem *mem,
                                                                 const struct kefir_ast_context *context) {
    struct kefir_ast_declarator *function1_decl =
        kefir_ast_declarator_function(mem, kefir_ast_declarator_identifier(mem, context->symbols, "fn"));

    struct kefir_ast_declaration *decl1 = kefir_ast_new_single_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "value"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10))), NULL);
    REQUIRE(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers,
                                                       kefir_ast_storage_class_specifier_static(mem)) == KEFIR_OK,
            NULL);
    REQUIRE(kefir_ast_declarator_specifier_list_append(mem, &decl1->specifiers, kefir_ast_type_specifier_int(mem)) ==
                KEFIR_OK,
            NULL);

    struct kefir_ast_declaration *decl2 = kefir_ast_new_single_declaration(
        mem, kefir_ast_declarator_identifier(mem, context->symbols, "increment"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))), NULL);
    REQUIRE(kefir_ast_declarator_specifier_list_append(mem, &decl2->specifiers, kefir_ast_type_specifier_int(mem)) ==
                KEFIR_OK,
            NULL);

    struct kefir_ast_compound_statement *function1_body = kefir_ast_new_compound_statement(mem);
    REQUIRE(kefir_list_insert_after(mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                                    KEFIR_AST_NODE_BASE(decl1)) == KEFIR_OK,
            NULL);
    REQUIRE(kefir_list_insert_after(mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                                    KEFIR_AST_NODE_BASE(decl2)) == KEFIR_OK,
            NULL);
    REQUIRE(kefir_list_insert_after(
                mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(
                             mem, KEFIR_AST_ASSIGNMENT_ADD,
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value")),
                             KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "increment"))))))) ==
                KEFIR_OK,
            NULL);
    REQUIRE(kefir_list_insert_after(
                mem, &function1_body->block_items, kefir_list_tail(&function1_body->block_items),
                KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                    mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "value"))))) == KEFIR_OK,
            NULL);

    struct kefir_ast_function_definition *function1 =
        kefir_ast_new_function_definition(mem, function1_decl, function1_body);
    REQUIRE(kefir_ast_declarator_specifier_list_append(mem, &function1->specifiers,
                                                       kefir_ast_type_specifier_int(mem)) == KEFIR_OK,
            NULL);

    return function1;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));

    struct kefir_ast_function_definition *function = define_sum_function(mem, &global_context.context);
    REQUIRE(function != NULL, KEFIR_INTERNAL_ERROR);

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(function)));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    translator_context.global_scope_layout = &global_scope;

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(kefir_ast_translate_function(mem, KEFIR_AST_NODE_BASE(function), &translator_context));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(function)));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
