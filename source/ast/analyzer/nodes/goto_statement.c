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

#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_analyze_goto_statement_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_goto_statement *node,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST goto statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(context->flow_control_tree != NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Goto statement is not allowed in current context"));

    if (node->base.klass->type == KEFIR_AST_GOTO_STATEMENT) {
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_OK(
            context->reference_label(mem, context, node->identifier, NULL, &node->base.source_location, &scoped_id));
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                       "Goto statement identifier should reference a label"));
        REQUIRE_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree,
                                                   &base->properties.statement_props.flow_control_statement));
        base->properties.statement_props.target_flow_control_point = scoped_id->label.point;
    } else {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->target));
        REQUIRE(node->target->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                       "Goto operand shall be either label identifier or dereferenced expression"));

        const struct kefir_ast_type *type1 =
            KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->target->properties.type);
        REQUIRE(type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->target->source_location,
                                       "Goto operand shall be derefenced pointer"));
    }

    REQUIRE_OK(
        context->current_flow_control_point(mem, context, &base->properties.statement_props.origin_flow_control_point));
    return KEFIR_OK;
}
