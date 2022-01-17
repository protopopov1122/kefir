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

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#define UNARY_NODE(_oper, _node)                                                                                      \
    do {                                                                                                              \
        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem, (_oper), (_node))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));                                                       \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));                         \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));                                                                   \
    } while (0)

#define BINARY_NODE(_oper, _node1, _node2)                                                         \
    do {                                                                                           \
        struct kefir_ast_node_base *node =                                                         \
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(mem, (_oper), (_node1), (_node2))); \
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));                                    \
        REQUIRE_OK(kefir_ast_translate_expression(mem, node, &builder, &translator_context));      \
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));                                                \
    } while (0)

#define FUNC(_id, _init)                                                                                           \
    do {                                                                                                           \
        kefir_id_t func_params_id, func_returns_id;                                                                \
        kefir_ir_module_new_type(mem, &module, 0, &func_params_id);                                                \
        kefir_ir_module_new_type(mem, &module, 0, &func_returns_id);                                               \
        struct kefir_ir_function_decl *func_decl =                                                                 \
            kefir_ir_module_new_function_declaration(mem, &module, (_id), func_params_id, false, func_returns_id); \
        REQUIRE(func_decl != NULL, KEFIR_INTERNAL_ERROR);                                                          \
        struct kefir_ir_function *func =                                                                           \
            kefir_ir_module_new_function(mem, &module, func_decl, translator_local_scope.local_layout, 0);         \
        REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));                                        \
        _init REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));                                                    \
    } while (0)

#define FUNC2(_id, _init)                                                                                            \
    do {                                                                                                             \
        struct kefir_ast_local_context local_context;                                                                \
        REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));                              \
        struct kefir_ast_context *context = &local_context.context;                                                  \
                                                                                                                     \
        struct kefir_ast_translator_local_scope_layout translator_local_scope;                                       \
        REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope,              \
                                                                &translator_local_scope));                           \
        struct kefir_ast_translator_context local_translator_context;                                                \
        REQUIRE_OK(kefir_ast_translator_context_init(mem, &local_translator_context, context, &env, &module, NULL)); \
        kefir_id_t func_params_id, func_returns_id;                                                                  \
        kefir_ir_module_new_type(mem, &module, 0, &func_params_id);                                                  \
        kefir_ir_module_new_type(mem, &module, 0, &func_returns_id);                                                 \
        struct kefir_ir_function_decl *func_decl =                                                                   \
            kefir_ir_module_new_function_declaration(mem, &module, (_id), func_params_id, false, func_returns_id);   \
        REQUIRE(func_decl != NULL, KEFIR_INTERNAL_ERROR);                                                            \
        struct kefir_ir_function *func =                                                                             \
            kefir_ir_module_new_function(mem, &module, func_decl, translator_local_scope.local_layout, 0);           \
        REQUIRE_OK(kefir_irbuilder_block_init(mem, &builder, &func->body));                                          \
        _init REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_FREE(&builder));                                                      \
        REQUIRE_OK(kefir_ast_translator_context_free(mem, &local_translator_context));                               \
        REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));                      \
        REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));                                               \
    } while (0)

#endif
