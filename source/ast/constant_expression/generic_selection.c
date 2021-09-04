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

#include "kefir/ast/constant_expression_impl.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/lang_error.h"

kefir_result_t kefir_ast_evaluate_generic_selection_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                         const struct kefir_ast_generic_selection *node,
                                                         struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST string literalnode"));
    REQUIRE(value != NULL,
            KEFIR_SET_LANG_ERROR(KEFIR_ANALYSIS_ERROR, NULL, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_LANG_ERROR(KEFIR_ANALYSIS_ERROR, NULL, "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_ERROR(KEFIR_NOT_CONSTANT, "Expected constant expression AST node"));

    const struct kefir_ast_type *control_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->control->properties.type);
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->associations); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc, iter->value);
        if (KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, control_type, assoc->type_name->base.properties.type)) {
            return kefir_ast_constant_expression_value_evaluate(mem, context, assoc->expr, value);
        }
    }
    if (node->default_assoc != NULL) {
        return kefir_ast_constant_expression_value_evaluate(mem, context, node->default_assoc, value);
    }

    return KEFIR_SET_LANG_ERROR(KEFIR_ANALYSIS_ERROR, NULL,
                                "Expected at least one of associations in generic selection to be compatible"
                                " with control expression type");
}
