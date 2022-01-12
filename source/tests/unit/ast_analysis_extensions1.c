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

#include "kefir/test/unit_test.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/test/util.h"

static kefir_result_t analyze_extension_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                             struct kefir_ast_node_base *node) {
    UNUSED(mem);
    UNUSED(context);
    node->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    node->properties.type = kefir_ast_type_signed_int();
    node->properties.expression_props.constant_expression = true;
    return KEFIR_OK;
}

DEFINE_CASE(ast_analysis_extension_node1, "AST analysis - extension node #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_ast_global_context context;

    struct kefir_ast_context_extensions ext = {.analyze_extension_node = analyze_extension_node};
    struct kefir_ast_extension_node_class ext_node_class = {0};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &env.target_env, &context, &ext));

    struct kefir_ast_extension_node *node1 = kefir_ast_new_extension_node(&kft_mem, &ext_node_class, NULL);
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, &context.context, KEFIR_AST_NODE_BASE(node1)));

    ASSERT(node1->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(node1->base.properties.type, kefir_ast_type_signed_int()));
    ASSERT(node1->base.properties.expression_props.constant_expression);
    ASSERT(!node1->base.properties.expression_props.addressable);
    ASSERT(!node1->base.properties.expression_props.bitfield);
    ASSERT(node1->base.properties.expression_props.identifier == NULL);
    ASSERT(!node1->base.properties.expression_props.lvalue);
    ASSERT(node1->base.properties.expression_props.scoped_id == NULL);
    ASSERT(node1->base.properties.expression_props.string_literal.content == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(node1)));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &context));
}
END_CASE

static kefir_result_t visit_identifier_node(const struct kefir_ast_visitor *visitor,
                                            const struct kefir_ast_identifier *node, void *payload) {
    UNUSED(visitor);
    UNUSED(node);
    ASSIGN_DECL_CAST(struct kefir_ast_analysis_parameters *, param, payload);

    param->base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;
    return KEFIR_OK;
}

static kefir_result_t before_node_analysis(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                           struct kefir_ast_node_base *node, struct kefir_ast_visitor *visitor) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(node);
    visitor->identifier = visit_identifier_node;
    return KEFIR_OK;
}

static kefir_result_t after_node_analysis(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                          struct kefir_ast_node_base *node) {
    UNUSED(mem);
    UNUSED(context);
    if (node->klass->type == KEFIR_AST_CONSTANT) {
        struct kefir_ast_constant *constant = node->self;
        if (constant->type == KEFIR_AST_UINT_CONSTANT) {
            node->properties.expression_props.constant_expression = false;
        }
    }
    return KEFIR_OK;
}

DEFINE_CASE(ast_analysis_before_after_extensions, "AST analysis - before & after extension hooks") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_translator_environment env;
    struct kefir_ast_global_context context;

    struct kefir_ast_context_extensions ext = {.before_node_analysis = before_node_analysis,
                                               .after_node_analysis = after_node_analysis};

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &env.target_env, &context, &ext));

    struct kefir_ast_identifier *node1 = kefir_ast_new_identifier(&kft_mem, &context.symbols, "X");
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, &context.context, KEFIR_AST_NODE_BASE(node1)));
    ASSERT(node1->base.properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT);

    struct kefir_ast_constant *node2 = kefir_ast_new_constant_int(&kft_mem, 1);
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, &context.context, KEFIR_AST_NODE_BASE(node2)));
    ASSERT(node2->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(node2->base.properties.type, kefir_ast_type_signed_int()));
    ASSERT(node2->base.properties.expression_props.constant_expression);

    struct kefir_ast_constant *node3 = kefir_ast_new_constant_uint(&kft_mem, 1);
    ASSERT_OK(kefir_ast_analyze_node(&kft_mem, &context.context, KEFIR_AST_NODE_BASE(node3)));
    ASSERT(node3->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);
    ASSERT(KEFIR_AST_TYPE_SAME(node3->base.properties.type, kefir_ast_type_unsigned_int()));
    ASSERT(!node3->base.properties.expression_props.constant_expression);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(node1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(node2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(node3)));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &context));
}
END_CASE
