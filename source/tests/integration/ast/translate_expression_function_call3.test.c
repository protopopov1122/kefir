/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *func_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_signed_int(), "sum", &func_type1);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type1, "x", NULL, NULL));
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type1, "y", NULL, NULL));
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type1, "z", NULL, NULL));

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_signed_int(), "sum2", &func_type2);

    struct kefir_ast_function_type *func_type3 = NULL;
    const struct kefir_ast_type *type3 =
        kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_signed_int(), "sum3", &func_type3);
    REQUIRE_OK(kefir_ast_type_function_parameter(mem, context->type_bundle, func_type3, "x", NULL, NULL));
    REQUIRE_OK(kefir_ast_type_function_ellipsis(func_type3, true));

    struct kefir_ast_type_name *type_name4 =
        kefir_ast_new_type_name(mem, kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                                                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)),
                                                                kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name4->type_decl.specifiers,
                                                          kefir_ast_type_specifier_char(mem)));

    REQUIRE_OK(kefir_ast_global_context_declare_function(mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type1,
                                                         NULL));
    REQUIRE_OK(kefir_ast_global_context_declare_function(mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type2,
                                                         NULL));
    REQUIRE_OK(kefir_ast_global_context_declare_function(mem, &global_context, KEFIR_AST_FUNCTION_SPECIFIER_NONE, type3,
                                                         NULL));

    struct kefir_ast_function_call *call5 =
        kefir_ast_new_function_call(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum3")));
    REQUIRE_OK(kefir_ast_function_call_append(mem, call5, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100))));
    REQUIRE_OK(kefir_ast_function_call_append(mem, call5, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 101))));
    REQUIRE_OK(
        kefir_ast_function_call_append(mem, call5, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 9.13))));
    REQUIRE_OK(kefir_ast_function_call_append(mem, call5,
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_compound_literal(mem, type_name4))));

    struct kefir_ast_node_base *node5 = KEFIR_AST_NODE_BASE(call5);
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node5));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(
        kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
        mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&translator_context),
        &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("call1", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum")));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100))));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 4.58f))));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, false))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call2", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum")));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100))));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 4.58f))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call3", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum2")));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 100))));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 101))));
        REQUIRE_OK(
            kefir_ast_function_call_append(mem, call, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 102))));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call4", {
        struct kefir_ast_function_call *call = kefir_ast_new_function_call(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "sum2")));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(call);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC("call5", {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node5, &builder, &translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node5));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
