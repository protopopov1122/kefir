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

#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_generic_selection_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                        const struct kefir_ast_generic_selection *node,
                                                        struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST generic selection"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->control));
    const struct kefir_ast_type *control_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->control->properties.type);

    kefir_bool_t matched = false;
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->associations); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(assoc->type_name)));
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, assoc->expr));
        if (!matched &&
            KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, control_type, assoc->type_name->base.properties.type)) {
            REQUIRE_OK(kefir_ast_node_properties_clone(&base->properties, &assoc->expr->properties));
            matched = true;
        }
    }
    if (!matched && node->default_assoc != NULL) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->default_assoc));
        REQUIRE_OK(kefir_ast_node_properties_clone(&base->properties, &node->default_assoc->properties));
        matched = true;
    }

    REQUIRE(matched, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                     "Expected at least one of associations in generic selection to be compatible"
                                     " with control expression type"));
    return KEFIR_OK;
}
