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

NODE_VISIT_IMPL(ast_expression_statement_visit, kefir_ast_expression_statement, expression_statement)

struct kefir_ast_node_base *ast_expression_statement_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_expression_statement_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_expression_statement *, node, base->self);
    if (node->expression != NULL) {
        KEFIR_AST_NODE_FREE(mem, node->expression);
    }
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_EXPRESSION_STATEMENT_CLASS = {.type = KEFIR_AST_EXPRESSION_STATEMENT,
                                                                    .visit = ast_expression_statement_visit,
                                                                    .clone = ast_expression_statement_clone,
                                                                    .free = ast_expression_statement_free};

struct kefir_ast_node_base *ast_expression_statement_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_expression_statement *, node, base->self);
    struct kefir_ast_expression_statement *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_expression_statement));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_EXPRESSION_STATEMENT_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    if (node->expression != NULL) {
        clone->expression = KEFIR_AST_NODE_CLONE(mem, node->expression);
        REQUIRE_ELSE(clone->expression != NULL, {
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    } else {
        clone->expression = NULL;
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_expression_statement *kefir_ast_new_expression_statement(struct kefir_mem *mem,
                                                                          struct kefir_ast_node_base *expression) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_expression_statement *expr_stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_expression_statement));
    REQUIRE(expr_stmt != NULL, NULL);
    expr_stmt->base.klass = &AST_EXPRESSION_STATEMENT_CLASS;
    expr_stmt->base.self = expr_stmt;
    kefir_result_t res = kefir_ast_node_properties_init(&expr_stmt->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, expr_stmt);
        return NULL;
    });
    res = kefir_source_location_empty(&expr_stmt->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, expr_stmt);
        return NULL;
    });
    expr_stmt->expression = expression;
    return expr_stmt;
}
