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

#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_static_assertion_visit, kefir_ast_static_assertion, static_assertion)

struct kefir_ast_node_base *ast_static_assertion_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_static_assertion_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_static_assertion *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->condition));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(node->string)));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_STATIC_ASSERTION_CLASS = {.type = KEFIR_AST_STATIC_ASSERTION,
                                                                .visit = ast_static_assertion_visit,
                                                                .clone = ast_static_assertion_clone,
                                                                .free = ast_static_assertion_free};

struct kefir_ast_node_base *ast_static_assertion_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_static_assertion *, node, base->self);
    struct kefir_ast_static_assertion *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_static_assertion));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_STATIC_ASSERTION_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->condition = KEFIR_AST_NODE_CLONE(mem, node->condition);
    REQUIRE_ELSE(clone->condition != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_static_assertion *kefir_ast_new_static_assertion(struct kefir_mem *mem,
                                                                  struct kefir_ast_node_base *condition,
                                                                  struct kefir_ast_string_literal *string) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(condition != NULL, NULL);
    REQUIRE(string != NULL, NULL);

    struct kefir_ast_static_assertion *id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_static_assertion));
    REQUIRE(id != NULL, NULL);
    id->base.klass = &AST_STATIC_ASSERTION_CLASS;
    id->base.self = id;
    kefir_result_t res = kefir_ast_node_properties_init(&id->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, id);
        return NULL;
    });

    id->condition = condition;
    id->string = string;
    return id;
}
