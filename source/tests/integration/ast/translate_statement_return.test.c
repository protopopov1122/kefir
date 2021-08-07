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
#include "kefir/ast-translator/flow_control.h"
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
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context, "variable", kefir_ast_type_signed_int(),
                                                         NULL, NULL));

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));

    struct kefir_ast_translator_context global_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&global_translator_context, &global_context.context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(
        mem, &module, &global_context, &env, kefir_ast_translator_context_type_resolver(&global_translator_context),
        &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC2("return0", {
        struct kefir_ast_function_type *function_type = NULL;
        const struct kefir_ast_type *type1 =
            kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_void(), "fn0", &function_type);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_OK(global_context.context.define_identifier(mem, &global_context.context, true, "fn0", type1,
                                                            KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                            KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, &scoped_id));
        local_context.context.surrounding_function = scoped_id;

        struct kefir_ast_return_statement *return1 = kefir_ast_new_return_statement(mem, NULL);

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(return1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, &local_context.context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC2("return1", {
        struct kefir_ast_function_type *function_type = NULL;
        const struct kefir_ast_type *type1 =
            kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_signed_long(), "fn1", &function_type);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_OK(global_context.context.define_identifier(mem, &global_context.context, true, "fn1", type1,
                                                            KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                            KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, &scoped_id));
        local_context.context.surrounding_function = scoped_id;

        struct kefir_ast_return_statement *return1 =
            kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10)));

        struct kefir_ast_return_statement *return2 =
            kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 3.14)));

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(return1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(return2)));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC2("return2", {
        struct kefir_ast_function_type *function_type = NULL;
        const struct kefir_ast_type *type1 =
            kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_float(), "fn2", &function_type);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_OK(global_context.context.define_identifier(mem, &global_context.context, true, "fn2", type1,
                                                            KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                            KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, &scoped_id));
        local_context.context.surrounding_function = scoped_id;

        struct kefir_ast_return_statement *return1 =
            kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10)));

        struct kefir_ast_return_statement *return2 =
            kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 3.14)));

        struct kefir_ast_compound_statement *compound1 = kefir_ast_new_compound_statement(mem);
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(return1)));
        REQUIRE_OK(kefir_list_insert_after(mem, &compound1->block_items, kefir_list_tail(&compound1->block_items),
                                           KEFIR_AST_NODE_BASE(return2)));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(compound1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC2("return3", {
        struct kefir_ast_type_name *type_name1 =
            kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL));
        struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(mem, NULL, NULL, true);
        struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                              kefir_ast_type_specifier_char(mem)));
        REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
            mem, entry1,
            kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 24)),
                                       kefir_ast_declarator_identifier(mem, context->symbols, "string")),
            NULL));
        REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name1->type_decl.specifiers,
                                                              kefir_ast_type_specifier_struct(mem, specifier1)));
        REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(type_name1)));

        struct kefir_ast_function_type *function_type = NULL;
        const struct kefir_ast_type *type1 =
            kefir_ast_type_function(mem, context->type_bundle, type_name1->base.properties.type, "fn3", &function_type);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_OK(global_context.context.define_identifier(mem, &global_context.context, true, "fn3", type1,
                                                            KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                            KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, &scoped_id));
        local_context.context.surrounding_function = scoped_id;

        struct kefir_ast_compound_literal *literal1 = kefir_ast_new_compound_literal(mem, type_name1);
        REQUIRE_OK(kefir_ast_initializer_list_append(
            mem, &literal1->initializer->list, NULL,
            kefir_ast_new_expression_initializer(mem,
                                                 KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, "Hey ho!")))));

        struct kefir_ast_return_statement *return1 = kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(literal1));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(return1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    FUNC2("return4", {
        struct kefir_ast_function_type *function_type = NULL;
        const struct kefir_ast_type *type1 =
            kefir_ast_type_function(mem, context->type_bundle, kefir_ast_type_char(), "fn4", &function_type);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_OK(global_context.context.define_identifier(mem, &global_context.context, true, "fn4", type1,
                                                            KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                                                            KEFIR_AST_FUNCTION_SPECIFIER_NONE, NULL, NULL, &scoped_id));
        local_context.context.surrounding_function = scoped_id;

        struct kefir_ast_return_statement *return1 =
            kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 6.1f)));

        struct kefir_ast_return_statement *return2 =
            kefir_ast_new_return_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 10)));

        struct kefir_ast_conditional_statement *conditional1 =
            kefir_ast_new_conditional_statement(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, true)),
                                                KEFIR_AST_NODE_BASE(return1), KEFIR_AST_NODE_BASE(return2));

        struct kefir_ast_do_while_statement *do_while1 = kefir_ast_new_do_while_statement(
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(mem, false)), KEFIR_AST_NODE_BASE(conditional1));

        struct kefir_ast_node_base *node = KEFIR_AST_NODE_BASE(do_while1);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));

        REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(
            mem, &local_context, &env, &module, kefir_ast_translator_context_type_resolver(&local_translator_context),
            &translator_local_scope));
        REQUIRE_OK(kefir_ast_translator_flow_control_tree_init(mem, context->flow_control_tree));

        REQUIRE_OK(kefir_ast_translate_statement(mem, node, &builder, &local_translator_context));
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &global_translator_context));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
