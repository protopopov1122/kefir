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
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/ast-translator/value.h"
#include "codegen.h"

static kefir_result_t analyze_function(struct kefir_mem *mem, struct function *func,
                                       struct kefir_ast_context_manager *context_manager) {
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));
    for (const struct kefir_list_entry *iter = kefir_list_head(&func->args); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, arg));
    }
    REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, func->body));
    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t translate_function(struct kefir_mem *mem, struct function *func,
                                         struct kefir_ast_context_manager *context_manager,
                                         struct kefir_ast_translator_global_scope_layout *global_scope,
                                         struct kefir_ast_translator_context *translator_context) {
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    struct kefir_ast_translator_function_declaration *func_declaration = NULL;
    REQUIRE_OK(kefir_ast_translator_function_declaration_init(
        mem, translator_context->environment, translator_context->ast_context->type_bundle,
        translator_context->ast_context->type_traits, translator_context->module, func->identifier, func->type, NULL,
        &func_declaration));

    struct kefir_ast_translator_context local_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init_local(mem, &local_translator_context, &func->local_context.context,
                                                       translator_context));

    struct kefir_ast_translator_local_scope_layout local_scope;
    REQUIRE_OK(
        kefir_ast_translator_local_scope_layout_init(mem, translator_context->module, global_scope, &local_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &func->local_context,
                                                             local_translator_context.environment,
                                                             local_translator_context.module, &local_scope));
    REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, func->local_context.context.flow_control_tree));

    struct kefir_ir_function *ir_func = kefir_ir_module_new_function(
        mem, local_translator_context.module, func_declaration->ir_function_decl, local_scope.local_layout_id, 0);

    struct kefir_irbuilder_block builder;
    REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &ir_func->body));
    for (const struct kefir_list_entry *iter = kefir_list_tail(&func->args); iter != NULL; iter = iter->prev) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, iter->value);
        REQUIRE_OK(kefir_ast_translate_lvalue(mem, &local_translator_context, &builder, arg));
        if (KEFIR_AST_TYPE_IS_LONG_DOUBLE(arg->properties.type)) {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_XCHG, 2));
        }
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(&builder, KEFIR_IROPCODE_XCHG, 1));
        REQUIRE_OK(kefir_ast_translator_store_value(mem, arg->properties.type, translator_context, &builder));
    }

    if (func->body->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION) {
        REQUIRE_OK(kefir_ast_translate_expression(mem, func->body, &builder, &local_translator_context));
    } else {
        REQUIRE_OK(kefir_ast_translate_statement(mem, func->body, &builder, &local_translator_context));
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));

    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &local_scope));
    REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, func_declaration));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &local_translator_context));
    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t free_function(struct kefir_mem *mem, struct function *func) {
    for (const struct kefir_list_entry *iter = kefir_list_head(&func->args); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, iter->value);
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, arg));
    }
    REQUIRE_OK(kefir_list_free(mem, &func->args));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, func->body));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &func->local_context));
    return KEFIR_OK;
}
