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

#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_cast_operator_visit, kefir_ast_cast_operator, cast_operator)

struct kefir_ast_node_base *ast_cast_operator_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_cast_operator_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_cast_operator *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(node->type_name)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->expr));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_CAST_OPERATOR_CLASS = {.type = KEFIR_AST_CAST_OPERATOR,
                                                             .visit = ast_cast_operator_visit,
                                                             .clone = ast_cast_operator_clone,
                                                             .free = ast_cast_operator_free};

struct kefir_ast_node_base *ast_cast_operator_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_cast_operator *, node, base->self);
    struct kefir_ast_cast_operator *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_cast_operator));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_CAST_OPERATOR_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    struct kefir_ast_node_base *clone_type_name = KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(node->type_name));
    REQUIRE_ELSE(clone_type_name != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->type_name = (struct kefir_ast_type_name *) clone_type_name->self;
    clone->expr = KEFIR_AST_NODE_CLONE(mem, node->expr);
    REQUIRE_ELSE(clone->expr != NULL, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(clone->type_name));
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_cast_operator *kefir_ast_new_cast_operator(struct kefir_mem *mem,
                                                            struct kefir_ast_type_name *type_name,
                                                            struct kefir_ast_node_base *expr) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type_name != NULL, NULL);
    REQUIRE(expr != NULL, NULL);
    struct kefir_ast_cast_operator *cast = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_cast_operator));
    REQUIRE(cast != NULL, NULL);
    cast->base.klass = &AST_CAST_OPERATOR_CLASS;
    cast->base.self = cast;
    kefir_result_t res = kefir_ast_node_properties_init(&cast->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, cast);
        return NULL;
    });
    cast->type_name = type_name;
    cast->expr = expr;
    return cast;
}
