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

#ifndef KEFIR_AST_NODE_INTERNAL_H_
#define KEFIR_AST_NODE_INTERNAL_H_

#include "kefir/core/basic-types.h"

#define NODE_VISIT_IMPL(identifier, type, handler)                                                             \
    kefir_result_t identifier(const struct kefir_ast_node_base *base, const struct kefir_ast_visitor *visitor, \
                              void *payload) {                                                                 \
        REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));           \
        REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node visitor"));     \
        ASSIGN_DECL_CAST(const struct type *, node, base->self);                                               \
        if (visitor->handler != NULL) {                                                                        \
            return visitor->handler(visitor, node, payload);                                                   \
        } else if (visitor->generic_handler != NULL) {                                                         \
            return visitor->generic_handler(visitor, base, payload);                                           \
        } else {                                                                                               \
            return KEFIR_OK;                                                                                   \
        }                                                                                                      \
    }

#endif
