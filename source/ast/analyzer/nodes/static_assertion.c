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
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_static_assertion_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_static_assertion *node,
                                                       struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST static assertion"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR;
    base->properties.declaration_props.static_assertion = true;
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->condition));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(node->string)));

    struct kefir_ast_constant_expression_value value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node->condition, &value));
    REQUIRE(
        value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Static assertion condition shall be an integral constant expression"));
    REQUIRE(value.integer != 0,
            KEFIR_SET_ERROR(KEFIR_STATIC_ASSERT, "Static assertion failure"));  // TODO Provide string from assertion
    return KEFIR_OK;
}
