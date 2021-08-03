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

NODE_VISIT_IMPL(ast_do_while_statement_visit, kefir_ast_do_while_statement, do_while_statement)

struct kefir_ast_node_base *ast_do_while_statement_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_do_while_statement_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valstmt memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valstmt AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_do_while_statement *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->controlling_expr));
    node->controlling_expr = NULL;
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->body));
    node->body = NULL;
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_DO_WHILE_STATEMENT_CLASS = {.type = KEFIR_AST_DO_WHILE_STATEMENT,
                                                                  .visit = ast_do_while_statement_visit,
                                                                  .clone = ast_do_while_statement_clone,
                                                                  .free = ast_do_while_statement_free};

struct kefir_ast_node_base *ast_do_while_statement_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_do_while_statement *, node, base->self);
    struct kefir_ast_do_while_statement *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_do_while_statement));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_DO_WHILE_STATEMENT_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->controlling_expr = KEFIR_AST_NODE_CLONE(mem, node->controlling_expr);
    REQUIRE_ELSE(clone->controlling_expr != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->body = KEFIR_AST_NODE_CLONE(mem, node->body);
    REQUIRE_ELSE(clone->body != NULL, {
        KEFIR_AST_NODE_FREE(mem, clone->controlling_expr);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_do_while_statement *kefir_ast_new_do_while_statement(struct kefir_mem *mem,
                                                                      struct kefir_ast_node_base *controlling_expr,
                                                                      struct kefir_ast_node_base *body) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(controlling_expr != NULL, NULL);
    REQUIRE(body != NULL, NULL);

    struct kefir_ast_do_while_statement *stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_do_while_statement));
    REQUIRE(stmt != NULL, NULL);
    stmt->base.klass = &AST_DO_WHILE_STATEMENT_CLASS;
    stmt->base.self = stmt;
    kefir_result_t res = kefir_ast_node_properties_init(&stmt->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, stmt);
        return NULL;
    });

    stmt->controlling_expr = controlling_expr;
    stmt->body = body;
    return stmt;
}
