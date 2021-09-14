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

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    const struct kefir_ast_context *context = &local_context.context;

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context, "int", kefir_ast_type_signed_int(), NULL, NULL,
                                                   NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context, "short", kefir_ast_type_unsigned_short(), NULL,
                                                   NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context, "float", kefir_ast_type_float(), NULL, NULL,
                                                   NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context, "double", kefir_ast_type_double(), NULL, NULL,
                                                   NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(
        mem, &local_context, "ptr1", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_bool()), NULL,
        NULL, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(
        mem, &local_context, "ptr2",
        kefir_ast_type_pointer(mem, context->type_bundle,
                               kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_signed_long(),
                                                    kefir_ast_constant_expression_integer(mem, 4), NULL)),
        NULL, NULL, NULL, NULL));

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

    FUNC("increment_int1", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")));
    });

    FUNC("increment_int2", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "short")));
    });

    FUNC("decrement_int1", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "int")));
    });

    FUNC("decrement_int2", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "short")));
    });

    FUNC("increment_float", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "float")));
    });

    FUNC("increment_double", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "double")));
    });

    FUNC("decrement_float", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "float")));
    });

    FUNC("decrement_double", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "double")));
    });

    FUNC("increment_pointer1", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
    });

    FUNC("increment_pointer2", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_INCREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
    });

    FUNC("decrement_pointer1", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr1")));
    });

    FUNC("decrement_pointer2", {
        UNARY_NODE(KEFIR_AST_OPERATION_POSTFIX_DECREMENT,
                   KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "ptr2")));
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
