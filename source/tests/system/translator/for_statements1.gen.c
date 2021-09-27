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

#include "codegen.inc.c"

static kefir_result_t define_conditional_function(struct kefir_mem *mem, struct function *func,
                                                  struct kefir_ast_context_manager *context_manager) {
    func->identifier = "sumall";
    REQUIRE_OK(kefir_list_init(&func->args));

    const struct kefir_ast_type *arg_type = kefir_ast_type_unbounded_array(mem, context_manager->current->type_bundle,
                                                                           kefir_ast_type_signed_long_long(), NULL);

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, kefir_ast_type_signed_long_long(),
                                         func->identifier, &func_type);
    REQUIRE_OK(
        kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL, arg_type, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        func->identifier, func->type, NULL, &scoped_id));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    func->local_context.context.surrounding_function = scoped_id;

    REQUIRE_OK(kefir_ast_local_context_define_auto(
        mem, context_manager->local, "values",
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context_manager->current->type_bundle, arg_type), NULL, NULL, NULL,
        NULL));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "values"))));

    struct kefir_ast_compound_statement *compound0 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_declaration *declarationSum = kefir_ast_new_single_declaration(
        mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "sum"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))), NULL);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &declarationSum->specifiers,
                                                          kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items),
                                       KEFIR_AST_NODE_BASE(declarationSum)));

    struct kefir_ast_declaration *loop_init = kefir_ast_new_single_declaration(
        mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, "i"),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))), NULL);
    REQUIRE_OK(
        kefir_ast_declarator_specifier_list_append(mem, &loop_init->specifiers, kefir_ast_type_specifier_int(mem)));

    struct kefir_ast_node_base *loop_tail = KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
        mem, KEFIR_AST_OPERATION_PREFIX_INCREMENT,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "i"))));

    struct kefir_ast_compound_statement *loop_body = kefir_ast_new_compound_statement(mem);

    struct kefir_ast_node_base *loop_stmt1 = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
        mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
            mem, KEFIR_AST_OPERATION_LOGICAL_AND,
            KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
                mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
                KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                    mem,
                    KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "values")),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "i")))))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                mem, KEFIR_AST_OPERATION_GREATER,
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "i")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_break_statement(mem)), NULL));
    REQUIRE_OK(
        kefir_list_insert_after(mem, &loop_body->block_items, kefir_list_tail(&loop_body->block_items), loop_stmt1));

    struct kefir_ast_node_base *loop_stmt2 = KEFIR_AST_NODE_BASE(kefir_ast_new_conditional_statement(
        mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            mem, KEFIR_AST_OPERATION_LOGICAL_NEGATE,
            KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "values")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "i")))))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_continue_statement(mem)), NULL));
    REQUIRE_OK(
        kefir_list_insert_after(mem, &loop_body->block_items, kefir_list_tail(&loop_body->block_items), loop_stmt2));

    struct kefir_ast_node_base *loop_stmt3 = KEFIR_AST_NODE_BASE(kefir_ast_new_expression_statement(
        mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(
            mem, KEFIR_AST_ASSIGNMENT_ADD,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sum")),
            KEFIR_AST_NODE_BASE(kefir_ast_new_array_subscript(
                mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "values")),
                KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "i"))))))));
    REQUIRE_OK(
        kefir_list_insert_after(mem, &loop_body->block_items, kefir_list_tail(&loop_body->block_items), loop_stmt3));

    struct kefir_ast_node_base *loop = KEFIR_AST_NODE_BASE(kefir_ast_new_for_statement(
        mem, KEFIR_AST_NODE_BASE(loop_init), NULL, loop_tail, KEFIR_AST_NODE_BASE(loop_body)));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items), loop));

    struct kefir_ast_return_statement *returnStatement = kefir_ast_new_return_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "sum")));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound0->block_items, kefir_list_tail(&compound0->block_items),
                                       KEFIR_AST_NODE_BASE(returnStatement)));

    func->body = KEFIR_AST_NODE_BASE(compound0);

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
    REQUIRE_OK(define_conditional_function(mem, &func, &context_manager));
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
