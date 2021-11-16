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
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct kefir_ast_struct_type *struct_type = NULL;
    const struct kefir_ast_type *struct_param_type =
        kefir_ast_type_structure(mem, context_manager.current->type_bundle, NULL, &struct_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager.current->symbols, struct_type, "x",
                                           kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(
        mem, context_manager.current->symbols, struct_type, "y",
        kefir_ast_type_array(mem, context_manager.current->type_bundle, kefir_ast_type_char(),
                             kefir_ast_constant_expression_integer(mem, 16), NULL),
        NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(
        mem, context_manager.current->symbols, struct_type, "z",
        kefir_ast_type_pointer(mem, context_manager.current->type_bundle, kefir_ast_type_void()), NULL));

    struct function simple_assign_long, simple_assign_struct, multiply_assign_int, multiply_assign_float,
        divide_assign_ulong, divide_assign_double, modulo_assign_short, shl_assign_uint, shr_assign_uint,
        iand_assign_long, ior_assign_long, ixor_assign_long, add_assign_int, add_assign_double, add_assign_floatptr,
        sub_assign_long_long, sub_assign_float, sub_assign_charptr;
    REQUIRE_OK(define_assign_function(mem, &simple_assign_long, &context_manager, "assign_long",
                                      kefir_ast_type_signed_long(), KEFIR_AST_ASSIGNMENT_SIMPLE));
    REQUIRE_OK(define_assign_function(mem, &simple_assign_struct, &context_manager, "assign_struct", struct_param_type,
                                      KEFIR_AST_ASSIGNMENT_SIMPLE));
    REQUIRE_OK(define_assign_function(mem, &multiply_assign_int, &context_manager, "multiply_assign_int",
                                      kefir_ast_type_signed_int(), KEFIR_AST_ASSIGNMENT_MULTIPLY));
    REQUIRE_OK(define_assign_function(mem, &multiply_assign_float, &context_manager, "multiply_assign_float",
                                      kefir_ast_type_float(), KEFIR_AST_ASSIGNMENT_MULTIPLY));
    REQUIRE_OK(define_assign_function(mem, &divide_assign_ulong, &context_manager, "divide_assign_ulong",
                                      kefir_ast_type_unsigned_long(), KEFIR_AST_ASSIGNMENT_DIVIDE));
    REQUIRE_OK(define_assign_function(mem, &divide_assign_double, &context_manager, "divide_assign_double",
                                      kefir_ast_type_double(), KEFIR_AST_ASSIGNMENT_DIVIDE));
    REQUIRE_OK(define_assign_function(mem, &modulo_assign_short, &context_manager, "modulo_assign_short",
                                      kefir_ast_type_signed_short(), KEFIR_AST_ASSIGNMENT_MODULO));
    REQUIRE_OK(define_assign_function(mem, &shl_assign_uint, &context_manager, "shl_assign_uint",
                                      kefir_ast_type_unsigned_int(), KEFIR_AST_ASSIGNMENT_SHIFT_LEFT));
    REQUIRE_OK(define_assign_function(mem, &shr_assign_uint, &context_manager, "shr_assign_uint",
                                      kefir_ast_type_unsigned_int(), KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT));
    REQUIRE_OK(define_assign_function(mem, &iand_assign_long, &context_manager, "iand_assign_long",
                                      kefir_ast_type_signed_long(), KEFIR_AST_ASSIGNMENT_BITWISE_AND));
    REQUIRE_OK(define_assign_function(mem, &ior_assign_long, &context_manager, "ior_assign_long",
                                      kefir_ast_type_signed_long(), KEFIR_AST_ASSIGNMENT_BITWISE_OR));
    REQUIRE_OK(define_assign_function(mem, &ixor_assign_long, &context_manager, "ixor_assign_long",
                                      kefir_ast_type_signed_long(), KEFIR_AST_ASSIGNMENT_BITWISE_XOR));
    REQUIRE_OK(define_assign_function(mem, &add_assign_int, &context_manager, "add_assign_int",
                                      kefir_ast_type_signed_int(), KEFIR_AST_ASSIGNMENT_ADD));
    REQUIRE_OK(define_assign_function(mem, &add_assign_double, &context_manager, "add_assign_double",
                                      kefir_ast_type_double(), KEFIR_AST_ASSIGNMENT_ADD));
    REQUIRE_OK(define_assign_function_impl(
        mem, &add_assign_floatptr, &context_manager, "add_assign_floatptr",
        kefir_ast_type_pointer(mem, context_manager.current->type_bundle, kefir_ast_type_float()),
        kefir_ast_type_signed_int(), KEFIR_AST_ASSIGNMENT_ADD));
    REQUIRE_OK(define_assign_function(mem, &sub_assign_long_long, &context_manager, "sub_assign_long_long",
                                      kefir_ast_type_signed_long_long(), KEFIR_AST_ASSIGNMENT_SUBTRACT));
    REQUIRE_OK(define_assign_function(mem, &sub_assign_float, &context_manager, "sub_assign_float",
                                      kefir_ast_type_float(), KEFIR_AST_ASSIGNMENT_SUBTRACT));
    REQUIRE_OK(define_assign_function_impl(
        mem, &sub_assign_charptr, &context_manager, "sub_assign_charptr",
        kefir_ast_type_pointer(mem, context_manager.current->type_bundle, kefir_ast_type_char()),
        kefir_ast_type_signed_int(), KEFIR_AST_ASSIGNMENT_SUBTRACT));

    REQUIRE_OK(analyze_function(mem, &simple_assign_long, &context_manager));
    REQUIRE_OK(analyze_function(mem, &simple_assign_struct, &context_manager));
    REQUIRE_OK(analyze_function(mem, &multiply_assign_int, &context_manager));
    REQUIRE_OK(analyze_function(mem, &multiply_assign_float, &context_manager));
    REQUIRE_OK(analyze_function(mem, &divide_assign_ulong, &context_manager));
    REQUIRE_OK(analyze_function(mem, &divide_assign_double, &context_manager));
    REQUIRE_OK(analyze_function(mem, &modulo_assign_short, &context_manager));
    REQUIRE_OK(analyze_function(mem, &shl_assign_uint, &context_manager));
    REQUIRE_OK(analyze_function(mem, &shr_assign_uint, &context_manager));
    REQUIRE_OK(analyze_function(mem, &iand_assign_long, &context_manager));
    REQUIRE_OK(analyze_function(mem, &ior_assign_long, &context_manager));
    REQUIRE_OK(analyze_function(mem, &ixor_assign_long, &context_manager));
    REQUIRE_OK(analyze_function(mem, &add_assign_int, &context_manager));
    REQUIRE_OK(analyze_function(mem, &add_assign_double, &context_manager));
    REQUIRE_OK(analyze_function(mem, &add_assign_floatptr, &context_manager));
    REQUIRE_OK(analyze_function(mem, &sub_assign_long_long, &context_manager));
    REQUIRE_OK(analyze_function(mem, &sub_assign_float, &context_manager));
    REQUIRE_OK(analyze_function(mem, &sub_assign_charptr, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(
        kefir_ast_translator_context_init(mem, &translator_context, &global_context.context, &env, module, NULL));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(translate_function(mem, &simple_assign_long, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &simple_assign_struct, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &multiply_assign_int, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &multiply_assign_float, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &divide_assign_ulong, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &divide_assign_double, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &modulo_assign_short, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &shl_assign_uint, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &shr_assign_uint, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &iand_assign_long, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &ior_assign_long, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &ixor_assign_long, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &add_assign_int, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &add_assign_double, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &add_assign_floatptr, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &sub_assign_long_long, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &sub_assign_float, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &sub_assign_charptr, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &simple_assign_long));
    REQUIRE_OK(free_function(mem, &simple_assign_struct));
    REQUIRE_OK(free_function(mem, &multiply_assign_int));
    REQUIRE_OK(free_function(mem, &multiply_assign_float));
    REQUIRE_OK(free_function(mem, &divide_assign_ulong));
    REQUIRE_OK(free_function(mem, &divide_assign_double));
    REQUIRE_OK(free_function(mem, &modulo_assign_short));
    REQUIRE_OK(free_function(mem, &shl_assign_uint));
    REQUIRE_OK(free_function(mem, &shr_assign_uint));
    REQUIRE_OK(free_function(mem, &iand_assign_long));
    REQUIRE_OK(free_function(mem, &ior_assign_long));
    REQUIRE_OK(free_function(mem, &ixor_assign_long));
    REQUIRE_OK(free_function(mem, &add_assign_int));
    REQUIRE_OK(free_function(mem, &add_assign_double));
    REQUIRE_OK(free_function(mem, &add_assign_floatptr));
    REQUIRE_OK(free_function(mem, &sub_assign_long_long));
    REQUIRE_OK(free_function(mem, &sub_assign_float));
    REQUIRE_OK(free_function(mem, &sub_assign_charptr));

    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
