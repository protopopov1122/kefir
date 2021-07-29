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

NODE_VISIT_IMPL(ast_string_literal_visit, kefir_ast_string_literal, string_literal)

struct kefir_ast_node_base *ast_string_literal_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_string_literal_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_string_literal *, node, base->self);
    KEFIR_FREE(mem, node->literal);
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_STRING_LITERAL_CLASS = {.type = KEFIR_AST_STRING_LITERAL,
                                                              .visit = ast_string_literal_visit,
                                                              .clone = ast_string_literal_clone,
                                                              .free = ast_string_literal_free};

struct kefir_ast_node_base *ast_string_literal_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_string_literal *, node, base->self);

    struct kefir_ast_string_literal *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_string_literal));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_STRING_LITERAL_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->literal = KEFIR_MALLOC(mem, node->length);
    REQUIRE_ELSE(clone->literal != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    memcpy(clone->literal, node->literal, node->length);
    clone->length = node->length;
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_string_literal *kefir_ast_new_string_literal(struct kefir_mem *mem, const char *literal,
                                                              kefir_size_t length) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(literal != NULL, NULL);

    char *literal_copy = KEFIR_MALLOC(mem, length);
    REQUIRE(literal_copy != NULL, NULL);

    struct kefir_ast_string_literal *string_literal = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_string_literal));
    REQUIRE_ELSE(string_literal != NULL, {
        KEFIR_FREE(mem, literal_copy);
        return NULL;
    });

    string_literal->base.klass = &AST_STRING_LITERAL_CLASS;
    string_literal->base.self = string_literal;
    kefir_result_t res = kefir_ast_node_properties_init(&string_literal->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, literal_copy);
        KEFIR_FREE(mem, string_literal);
        return NULL;
    });

    memcpy(literal_copy, literal, length);
    string_literal->literal = literal_copy;
    string_literal->length = length;
    return string_literal;
}
