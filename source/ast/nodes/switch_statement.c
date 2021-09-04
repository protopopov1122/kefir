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

NODE_VISIT_IMPL(ast_switch_statement_visit, kefir_ast_switch_statement, switch_statement)

struct kefir_ast_node_base *ast_switch_statement_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_switch_statement_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_switch_statement *, node, base->self);
    KEFIR_AST_NODE_FREE(mem, node->expression);
    KEFIR_AST_NODE_FREE(mem, node->statement);
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_SWITCH_STATEMENT_CLASS = {.type = KEFIR_AST_SWITCH_STATEMENT,
                                                                .visit = ast_switch_statement_visit,
                                                                .clone = ast_switch_statement_clone,
                                                                .free = ast_switch_statement_free};

struct kefir_ast_node_base *ast_switch_statement_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_switch_statement *, node, base->self);
    struct kefir_ast_switch_statement *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_switch_statement));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_SWITCH_STATEMENT_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->expression = KEFIR_AST_NODE_CLONE(mem, node->expression);
    REQUIRE_ELSE(clone->expression != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->statement = KEFIR_AST_NODE_CLONE(mem, node->statement);
    REQUIRE_ELSE(clone->statement != NULL, {
        KEFIR_AST_NODE_FREE(mem, clone->expression);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_switch_statement *kefir_ast_new_switch_statement(struct kefir_mem *mem,
                                                                  struct kefir_ast_node_base *expression,
                                                                  struct kefir_ast_node_base *statement) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(expression != NULL, NULL);
    REQUIRE(statement != NULL, NULL);

    struct kefir_ast_switch_statement *stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_switch_statement));
    REQUIRE(stmt != NULL, NULL);
    stmt->base.klass = &AST_SWITCH_STATEMENT_CLASS;
    stmt->base.self = stmt;
    kefir_result_t res = kefir_ast_node_properties_init(&stmt->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, stmt);
        return NULL;
    });
    res = kefir_source_location_empty(&stmt->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, stmt);
        return NULL;
    });

    stmt->expression = expression;
    stmt->statement = statement;
    return stmt;
}
