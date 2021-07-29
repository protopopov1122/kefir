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

#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/initializer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_compound_literal_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_compound_literal *node,
                                                       struct kefir_ast_node_base *base) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST compound literal"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(node->type_name)));
    const struct kefir_ast_type *unqualified_type = kefir_ast_unqualified_type(node->type_name->base.properties.type);
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(unqualified_type) ||
                (unqualified_type->tag == KEFIR_AST_TYPE_ARRAY &&
                 unqualified_type->array_type.boundary == KEFIR_AST_ARRAY_UNBOUNDED),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                            "Compound literal type shall be complete object or array of unknown length"));
    struct kefir_ast_initializer_properties initializer_properties;
    REQUIRE_OK(kefir_ast_analyze_initializer(mem, context, node->type_name->base.properties.type, node->initializer,
                                             &initializer_properties));
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    REQUIRE_OK(context->allocate_temporary_value(mem, context, initializer_properties.type,
                                                 &base->properties.expression_props.temporary));

    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = initializer_properties.type;
    base->properties.expression_props.lvalue = true;
    base->properties.expression_props.addressable = true;
    base->properties.expression_props.constant_expression = initializer_properties.constant;
    return KEFIR_OK;
}
