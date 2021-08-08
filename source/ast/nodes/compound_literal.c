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
#include "kefir/ast/node_helpers.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_compound_literal_visit, kefir_ast_compound_literal, compound_literal)

struct kefir_ast_node_base *ast_compound_literal_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_compound_literal_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_compound_literal *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(node->type_name)));
    REQUIRE_OK(kefir_ast_initializer_free(mem, node->initializer));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_COMPOUND_LITERAL_CLASS = {.type = KEFIR_AST_COMPOUND_LITERAL,
                                                                .visit = ast_compound_literal_visit,
                                                                .clone = ast_compound_literal_clone,
                                                                .free = ast_compound_literal_free};

struct kefir_ast_node_base *ast_compound_literal_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_compound_literal *, node, base->self);
    struct kefir_ast_compound_literal *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_compound_literal));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_COMPOUND_LITERAL_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    struct kefir_ast_node_base *type_name_clone = KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(node->type_name));
    REQUIRE_ELSE(type_name_clone != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->type_name = (struct kefir_ast_type_name *) type_name_clone->self;
    clone->initializer = kefir_ast_initializer_clone(mem, node->initializer);
    REQUIRE_ELSE(clone->initializer != NULL, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(clone->type_name));
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_compound_literal *kefir_ast_new_compound_literal(struct kefir_mem *mem,
                                                                  struct kefir_ast_type_name *type_name) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type_name != NULL, NULL);

    struct kefir_ast_compound_literal *literal = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_compound_literal));
    REQUIRE(literal != NULL, NULL);
    literal->base.klass = &AST_COMPOUND_LITERAL_CLASS;
    literal->base.self = literal;
    kefir_result_t res = kefir_ast_node_properties_init(&literal->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, literal);
        return NULL;
    });
    literal->type_name = type_name;
    literal->initializer = kefir_ast_new_list_initializer(mem);
    REQUIRE_ELSE(literal->initializer != NULL, {
        KEFIR_FREE(mem, literal);
        return NULL;
    });
    return literal;
}

kefir_result_t kefir_ast_compound_literal_set_initializer(struct kefir_mem *mem,
                                                          struct kefir_ast_compound_literal *literal,
                                                          struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(literal != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST compound literal"));
    REQUIRE(initializer != NULL && initializer->type == KEFIR_AST_INITIALIZER_LIST,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST list initializer"));

    REQUIRE_OK(kefir_ast_initializer_free(mem, literal->initializer));
    literal->initializer = initializer;
    return KEFIR_OK;
}
