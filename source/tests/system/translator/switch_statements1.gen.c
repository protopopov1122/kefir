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
#include "kefir/test/util.h"

#include "codegen.inc.c"

static kefir_result_t define_conditional_function(struct kefir_mem *mem, struct function *func,
                                                  struct kefir_ast_context_manager *context_manager) {
    func->identifier = "char_to_int";
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    func->type =
        kefir_ast_type_function(mem, context_manager->current->type_bundle, kefir_ast_type_signed_int(), &func_type);
    REQUIRE_OK(kefir_ast_type_function_parameter(
        mem, context_manager->current->type_bundle, func_type,
        kefir_ast_type_pointer(mem, context_manager->current->type_bundle, kefir_ast_type_char()), NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        func->identifier, func->type, NULL, &scoped_id));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    func->local_context.context.surrounding_function = scoped_id;

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "chr", kefir_ast_type_char(), NULL,
                                                   NULL, NULL, NULL));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "chr"))));

    struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
    struct kefir_ast_switch_statement *switch1 = kefir_ast_new_switch_statement(
        mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "chr")),
        KEFIR_AST_NODE_BASE(compound1));

    struct kefir_ast_node_base *case0 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '0')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case0));

    struct kefir_ast_node_base *case1 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '1')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case1));

    struct kefir_ast_node_base *case2 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '2')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case2));

    struct kefir_ast_node_base *case3 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '3')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case3));

    struct kefir_ast_node_base *case4 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '4')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case4));

    struct kefir_ast_node_base *case5 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '5')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case5));

    struct kefir_ast_node_base *case6 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '6')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 6))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case6));

    struct kefir_ast_node_base *case7 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '7')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 7))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case7));

    struct kefir_ast_node_base *case8 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '8')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 8))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case8));

    struct kefir_ast_node_base *case9 = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, '9')),
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 9))))));
    REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), case9));

    struct kefir_ast_node_base *caseDefault = KEFIR_AST_NODE_BASE(
        kefir_ast_new_case_statement(mem, NULL,
                                     KEFIR_AST_NODE_BASE(kefir_ast_new_return_statement(
                                         mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1))))));
    REQUIRE_OK(
        kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items), caseDefault));

    func->body = KEFIR_AST_NODE_BASE(switch1);

    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_util_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function func;
    REQUIRE_OK(define_conditional_function(mem, &func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &func, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(
        kefir_ast_translator_context_init(mem, &translator_context, &global_context.context, &env, module, NULL));

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
