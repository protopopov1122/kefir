/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

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

#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t match_switch_statement(const struct kefir_ast_flow_control_statement *statement, void *payload,
                                             kefir_bool_t *result) {
    UNUSED(payload);
    REQUIRE(statement != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST flow control statement"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid pointer to result"));

    *result = statement->type == KEFIR_AST_FLOW_CONTROL_STATEMENT_SWITCH;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_case_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_case_statement *node,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST case statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to use switch statement in current context"));

    struct kefir_ast_flow_control_statement *switch_statement = NULL;
    REQUIRE_OK(kefir_ast_flow_control_tree_traverse(context->flow_control_tree, match_switch_statement, NULL,
                                                    &switch_statement));
    base->properties.statement_props.flow_control_statement = switch_statement;

    if (node->expression != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->expression));
        struct kefir_ast_constant_expression_value value;
        REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->expression, &value));
        REQUIRE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST case label to be an integral constant expression"));
        struct kefir_ast_flow_control_point *point = kefir_ast_flow_control_point_alloc(mem);
        REQUIRE(point != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control point"));
        kefir_result_t res =
            kefir_hashtree_insert(mem, &switch_statement->value.switchStatement.cases,
                                  (kefir_hashtree_key_t) value.integer, (kefir_hashtree_value_t) point);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_flow_control_point_free(mem, point);
            return res;
        });
        base->properties.statement_props.flow_control_point = point;
    } else {
        REQUIRE(switch_statement->value.switchStatement.defaultCase == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Switch statement cannot have multiple default labels"));
        switch_statement->value.switchStatement.defaultCase = kefir_ast_flow_control_point_alloc(mem);
        REQUIRE(switch_statement->value.switchStatement.defaultCase != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control point"));
        base->properties.statement_props.flow_control_point = switch_statement->value.switchStatement.defaultCase;
    }

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->statement));
    REQUIRE(node->statement->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST statement node to be associated with the case"));
    return KEFIR_OK;
}
