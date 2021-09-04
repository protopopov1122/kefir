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

#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_visitor_init(struct kefir_ast_visitor *visitor,
                                      kefir_result_t (*generic)(const struct kefir_ast_visitor *,
                                                                const struct kefir_ast_node_base *, void *)) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST visitor"));
    *visitor = (const struct kefir_ast_visitor){0};
    visitor->generic_handler = generic;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_node_properties_init(struct kefir_ast_node_properties *props) {
    REQUIRE(props != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node properties pointer"));
    *props = (struct kefir_ast_node_properties){0};
    props->category = KEFIR_AST_NODE_CATEGORY_UNKNOWN;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_node_properties_clone(struct kefir_ast_node_properties *dst_props,
                                               const struct kefir_ast_node_properties *src_props) {
    REQUIRE(dst_props != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node properties destination pointer"));
    REQUIRE(src_props != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node properties source pointer"));
    dst_props->category = src_props->category;
    dst_props->type = src_props->type;
    dst_props->expression_props = src_props->expression_props;
    return KEFIR_OK;
}
