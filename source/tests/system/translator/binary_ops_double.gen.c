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

#define DEFFUN(_id, _name, _oper, _ret)                                                                               \
    static kefir_result_t define_##_id##_function(struct kefir_mem *mem, struct function *func,                       \
                                                  struct kefir_ast_context_manager *context_manager) {                \
        func->identifier = (_name);                                                                                   \
        REQUIRE_OK(kefir_list_init(&func->args));                                                                     \
                                                                                                                      \
        struct kefir_ast_function_type *func_type = NULL;                                                             \
        func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, (_ret), &func_type);         \
        REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL,     \
                                                     kefir_ast_type_double(), NULL));                                 \
        REQUIRE_OK(kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, NULL,     \
                                                     kefir_ast_type_double(), NULL));                                 \
                                                                                                                      \
        REQUIRE_OK(kefir_ast_global_context_define_function(                                                          \
            mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE, (_name), func->type, NULL, NULL));       \
                                                                                                                      \
        REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));                 \
        REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));                    \
                                                                                                                      \
        REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "x", kefir_ast_type_double(),     \
                                                       NULL, NULL, NULL, NULL));                                      \
        REQUIRE_OK(kefir_ast_local_context_define_auto(mem, context_manager->local, "y", kefir_ast_type_double(),     \
                                                       NULL, NULL, NULL, NULL));                                      \
                                                                                                                      \
        REQUIRE_OK(kefir_list_insert_after(                                                                           \
            mem, &func->args, kefir_list_tail(&func->args),                                                           \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x"))));             \
        REQUIRE_OK(kefir_list_insert_after(                                                                           \
            mem, &func->args, kefir_list_tail(&func->args),                                                           \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "y"))));             \
                                                                                                                      \
        func->body = KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(                                              \
            mem, (_oper), KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "x")), \
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "y"))));             \
                                                                                                                      \
        REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));                                          \
        return KEFIR_OK;                                                                                              \
    }

DEFFUN(sum, "sum", KEFIR_AST_OPERATION_ADD, kefir_ast_type_double())
DEFFUN(sub, "sub", KEFIR_AST_OPERATION_SUBTRACT, kefir_ast_type_double())
DEFFUN(mul, "mul", KEFIR_AST_OPERATION_MULTIPLY, kefir_ast_type_double())
DEFFUN(div, "divide", KEFIR_AST_OPERATION_DIVIDE, kefir_ast_type_double())
DEFFUN(less, "less", KEFIR_AST_OPERATION_LESS, kefir_ast_type_signed_int())
DEFFUN(lesseq, "lesseq", KEFIR_AST_OPERATION_LESS_EQUAL, kefir_ast_type_signed_int())
DEFFUN(greater, "greater", KEFIR_AST_OPERATION_GREATER, kefir_ast_type_signed_int())
DEFFUN(greatereq, "greatereq", KEFIR_AST_OPERATION_GREATER_EQUAL, kefir_ast_type_signed_int())
DEFFUN(equals, "equals", KEFIR_AST_OPERATION_EQUAL, kefir_ast_type_signed_int())
DEFFUN(noteq, "noteq", KEFIR_AST_OPERATION_NOT_EQUAL, kefir_ast_type_signed_int())

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function sum_func, sub_func, mul_func, div_func, less_func, lesseq_func, greater_func, greatereq_func,
        equals_func, noteq_func;
    REQUIRE_OK(define_sum_function(mem, &sum_func, &context_manager));
    REQUIRE_OK(define_sub_function(mem, &sub_func, &context_manager));
    REQUIRE_OK(define_mul_function(mem, &mul_func, &context_manager));
    REQUIRE_OK(define_div_function(mem, &div_func, &context_manager));
    REQUIRE_OK(define_less_function(mem, &less_func, &context_manager));
    REQUIRE_OK(define_lesseq_function(mem, &lesseq_func, &context_manager));
    REQUIRE_OK(define_greater_function(mem, &greater_func, &context_manager));
    REQUIRE_OK(define_greatereq_function(mem, &greatereq_func, &context_manager));
    REQUIRE_OK(define_equals_function(mem, &equals_func, &context_manager));
    REQUIRE_OK(define_noteq_function(mem, &noteq_func, &context_manager));

    REQUIRE_OK(analyze_function(mem, &sum_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &sub_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &mul_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &div_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &less_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &lesseq_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &greater_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &greatereq_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &equals_func, &context_manager));
    REQUIRE_OK(analyze_function(mem, &noteq_func, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));

    REQUIRE_OK(translate_function(mem, &sum_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &sub_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &mul_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &div_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &less_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &lesseq_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &greater_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &greatereq_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &equals_func, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &noteq_func, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &sum_func));
    REQUIRE_OK(free_function(mem, &sub_func));
    REQUIRE_OK(free_function(mem, &mul_func));
    REQUIRE_OK(free_function(mem, &div_func));
    REQUIRE_OK(free_function(mem, &less_func));
    REQUIRE_OK(free_function(mem, &lesseq_func));
    REQUIRE_OK(free_function(mem, &greater_func));
    REQUIRE_OK(free_function(mem, &greatereq_func));
    REQUIRE_OK(free_function(mem, &equals_func));
    REQUIRE_OK(free_function(mem, &noteq_func));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
