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
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"

#include "codegen.inc.c"

static kefir_result_t define_factorial_function(struct kefir_mem *mem, struct function *func,
                                                struct kefir_ast_context_manager *context_manager) {
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, kefir_ast_type_signed_int(),
                                         "factorial", &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL,
                                                 kefir_ast_type_signed_int(), NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        func->type, NULL, &scoped_id));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    func->local_context.context.surrounding_function = scoped_id;

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "x", kefir_ast_type_signed_int(), NULL,
                                                   NULL, NULL));

    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x"))));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);

    struct kefir_ast_node_base *condition = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_LESS,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2))));

    struct kefir_ast_node_base *conditionThen = KEFIR_AST_NODE_BASE(
        kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))));

    struct kefir_ast_node_base *conditionStatement =
        KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(mem, condition, conditionThen, NULL));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                       conditionStatement));

    struct kefir_ast_declaration *declarationResult = kefir_ast_new_single_declaration(
        mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "result"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationResult->specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                       KEFIR_AST_NODE_BASE(declarationResult)));

    struct kefir_ast_node_base *loopCondition = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
        mem, KEFIR_AST_OPERATION_GREATER,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))));

    struct kefir_ast_node_base *loopTail = KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
        mem, KEFIR_AST_OPERATION_PREFIX_DECREMENT,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x"))));

    struct kefir_ast_node_base *loopBody = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(
                 mem, KEFIR_AST_ASSIGNMENT_MULTIPLY,
                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "result")),
                 KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x"))))));

    struct kefir_ast_node_base *loop =
        KEFIR_AST_NODE_BASE(kefir_ast_new_for_statement(mem, NULL, loopCondition, loopTail, loopBody));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), loop));

    struct kefir_ast_node_base *returnResult = KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "result"))));
    REQUIRE_OK(
        kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), returnResult));

    func->body = KEFIR_AST_NODE_BASE(compound1);

    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function func;
    REQUIRE_OK(define_factorial_function(mem, &func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &func, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(translate_function(mem, &func, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &func));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
