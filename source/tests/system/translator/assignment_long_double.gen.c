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

static kefir_result_t define_assign_function_impl(struct kefir_mem *mem, struct function *func,
                                                  struct kefir_ast_context_manager *context_manager, const char *name,
                                                  const struct kefir_ast_type *param_type,
                                                  const struct kefir_ast_type *param2_type,
                                                  kefir_ast_assignment_operation_t oper) {
    REQUIRE_OK(kefir_list_init(&func->args));

    const struct kefir_ast_type *ptr_type =
        kefir_ast_type_pointer(mem, context_manager->current->type_bundle, param_type);

    struct kefir_ast_function_type *func_type = NULL;
    func->identifier = name;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, param_type, &func_type);
    REQUIRE_OK(
        kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, ptr_type, NULL));
    REQUIRE_OK(
        kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, param2_type, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        name, func->type, NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(
        kefir_ast_local_context_define_auto(mem, context_manager->local, "ptr", ptr_type, NULL, NULL, NULL, NULL));
    REQUIRE_OK(
        kefir_ast_local_context_define_auto(mem, context_manager->local, "value", param2_type, NULL, NULL, NULL, NULL));

    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "ptr"))));
    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value"))));

    func->body = KEFIR_AST_NODE_BASE(kefir_ast_new_compound_assignment(
        mem, oper,
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(
            mem, KEFIR_AST_OPERATION_INDIRECTION,
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "ptr")))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value"))));

    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t define_assign_function(struct kefir_mem *mem, struct function *func,
                                             struct kefir_ast_context_manager *context_manager, const char *name,
                                             const struct kefir_ast_type *param_type,
                                             kefir_ast_assignment_operation_t oper) {
    return define_assign_function_impl(mem, func, context_manager, name, param_type, param_type, oper);
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function simple_assign, multiply_assign, divide_assign, add_assign, sub_assign;
    REQUIRE_OK(define_assign_function(mem, &simple_assign, &context_manager, "assign", kefir_ast_type_long_double(),
                                      KEFIR_AST_ASSIGNMENT_SIMPLE));
    REQUIRE_OK(define_assign_function(mem, &multiply_assign, &context_manager, "multiply_assign",
                                      kefir_ast_type_long_double(), KEFIR_AST_ASSIGNMENT_MULTIPLY));
    REQUIRE_OK(define_assign_function(mem, &divide_assign, &context_manager, "divide_assign",
                                      kefir_ast_type_long_double(), KEFIR_AST_ASSIGNMENT_DIVIDE));
    REQUIRE_OK(define_assign_function(mem, &add_assign, &context_manager, "add_assign", kefir_ast_type_long_double(),
                                      KEFIR_AST_ASSIGNMENT_ADD));
    REQUIRE_OK(define_assign_function(mem, &sub_assign, &context_manager, "sub_assign", kefir_ast_type_long_double(),
                                      KEFIR_AST_ASSIGNMENT_SUBTRACT));

    REQUIRE_OK(analyze_function(mem, &simple_assign, &context_manager));
    REQUIRE_OK(analyze_function(mem, &multiply_assign, &context_manager));
    REQUIRE_OK(analyze_function(mem, &divide_assign, &context_manager));
    REQUIRE_OK(analyze_function(mem, &add_assign, &context_manager));
    REQUIRE_OK(analyze_function(mem, &sub_assign, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(translate_function(mem, &simple_assign, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &multiply_assign, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &divide_assign, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &add_assign, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &sub_assign, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &simple_assign));
    REQUIRE_OK(free_function(mem, &multiply_assign));
    REQUIRE_OK(free_function(mem, &divide_assign));
    REQUIRE_OK(free_function(mem, &add_assign));
    REQUIRE_OK(free_function(mem, &sub_assign));

    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
