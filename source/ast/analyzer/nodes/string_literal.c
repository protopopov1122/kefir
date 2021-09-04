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
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static const struct kefir_ast_type *string_element_type(const struct kefir_ast_type_traits *type_traits,
                                                        kefir_ast_string_literal_type_t type) {
    switch (type) {
        case KEFIR_AST_STRING_LITERAL_MULTIBYTE:
        case KEFIR_AST_STRING_LITERAL_UNICODE8:
            return kefir_ast_type_char();

        case KEFIR_AST_STRING_LITERAL_UNICODE16:
            return type_traits->unicode16_char_type;

        case KEFIR_AST_STRING_LITERAL_UNICODE32:
            return type_traits->unicode32_char_type;

        case KEFIR_AST_STRING_LITERAL_WIDE:
            return type_traits->wide_char_type;
    }
    return NULL;
}

kefir_result_t kefir_ast_analyze_string_literal_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                     const struct kefir_ast_string_literal *node,
                                                     struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST string literal"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    const struct kefir_ast_type *elt_type = string_element_type(context->type_traits, node->type);
    REQUIRE(elt_type != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to detect AST string literal underlying type"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = kefir_ast_type_array(mem, context->type_bundle, elt_type,
                                                 kefir_ast_constant_expression_integer(mem, node->length), NULL);
    base->properties.expression_props.constant_expression = true;
    base->properties.expression_props.string_literal.type = node->type;
    base->properties.expression_props.string_literal.content = node->literal;
    base->properties.expression_props.string_literal.length = node->length;
    return KEFIR_OK;
}
