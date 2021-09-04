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
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_array_subscript_visit, kefir_ast_array_subscript, array_subscript)

struct kefir_ast_node_base *ast_array_subscript_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_array_subscript_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_array_subscript *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->array));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->subscript));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_ARRAY_SUBSCRIPT_CLASS = {.type = KEFIR_AST_ARRAY_SUBSCRIPT,
                                                               .visit = ast_array_subscript_visit,
                                                               .clone = ast_array_subscript_clone,
                                                               .free = ast_array_subscript_free};

struct kefir_ast_node_base *ast_array_subscript_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_array_subscript *, node, base->self);
    struct kefir_ast_array_subscript *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_array_subscript));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_ARRAY_SUBSCRIPT_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->array = KEFIR_AST_NODE_CLONE(mem, node->array);
    REQUIRE_ELSE(clone->array != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->subscript = KEFIR_AST_NODE_CLONE(mem, node->subscript);
    REQUIRE_ELSE(clone->subscript != NULL, {
        KEFIR_AST_NODE_FREE(mem, clone->array);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_array_subscript *kefir_ast_new_array_subscript(struct kefir_mem *mem,
                                                                struct kefir_ast_node_base *array,
                                                                struct kefir_ast_node_base *subscript) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(array != NULL, NULL);
    REQUIRE(subscript != NULL, NULL);
    struct kefir_ast_array_subscript *array_subscript = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_array_subscript));
    REQUIRE(array_subscript != NULL, NULL);
    array_subscript->base.klass = &AST_ARRAY_SUBSCRIPT_CLASS;
    array_subscript->base.self = array_subscript;
    kefir_result_t res = kefir_ast_node_properties_init(&array_subscript->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, array_subscript);
        return NULL;
    });
    res = kefir_source_location_empty(&array_subscript->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, array_subscript);
        return NULL;
    });
    array_subscript->array = array;
    array_subscript->subscript = subscript;
    return array_subscript;
}
