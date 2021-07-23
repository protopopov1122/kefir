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

#include "kefir/ast/analyzer/nodes.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_constant_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                               const struct kefir_ast_constant *node,
                                               struct kefir_ast_node_base *base) {
    UNUSED(mem);
    UNUSED(context);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.expression_props.constant_expression = true;
    switch (node->type) {
        case KEFIR_AST_BOOL_CONSTANT:
            base->properties.type = kefir_ast_type_bool();
            break;

        case KEFIR_AST_CHAR_CONSTANT:
            base->properties.type = kefir_ast_type_unsigned_char();
            break;

        case KEFIR_AST_INT_CONSTANT:
            base->properties.type = kefir_ast_type_signed_int();
            break;

        case KEFIR_AST_UINT_CONSTANT:
            base->properties.type = kefir_ast_type_unsigned_int();
            break;

        case KEFIR_AST_LONG_CONSTANT:
            base->properties.type = kefir_ast_type_signed_long();
            break;

        case KEFIR_AST_ULONG_CONSTANT:
            base->properties.type = kefir_ast_type_unsigned_long();
            break;

        case KEFIR_AST_LONG_LONG_CONSTANT:
            base->properties.type = kefir_ast_type_signed_long_long();
            break;

        case KEFIR_AST_ULONG_LONG_CONSTANT:
            base->properties.type = kefir_ast_type_unsigned_long_long();
            break;

        case KEFIR_AST_FLOAT_CONSTANT:
            base->properties.type = kefir_ast_type_float();
            break;

        case KEFIR_AST_DOUBLE_CONSTANT:
            base->properties.type = kefir_ast_type_double();
            break;
    }
    return KEFIR_OK;
}
