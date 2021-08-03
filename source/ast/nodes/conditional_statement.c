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

NODE_VISIT_IMPL(ast_conditional_statement_visit, kefir_ast_conditional_statement, conditional_statement)

struct kefir_ast_node_base *ast_conditional_statement_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_conditional_statement_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_conditional_statement *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->condition));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->thenBranch));
    if (node->elseBranch != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->elseBranch));
    }
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_CONDITIONAL_STATEMENT_CLASS = {.type = KEFIR_AST_CONDITIONAL_STATEMENT,
                                                                     .visit = ast_conditional_statement_visit,
                                                                     .clone = ast_conditional_statement_clone,
                                                                     .free = ast_conditional_statement_free};

struct kefir_ast_node_base *ast_conditional_statement_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_conditional_statement *, node, base->self);
    struct kefir_ast_conditional_statement *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_conditional_statement));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_CONDITIONAL_STATEMENT_CLASS;
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

    clone->thenBranch = KEFIR_AST_NODE_CLONE(mem, node->thenBranch);
    REQUIRE_ELSE(clone->thenBranch != NULL, {
        KEFIR_AST_NODE_FREE(mem, clone->condition);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    if (node->elseBranch != NULL) {
        clone->elseBranch = KEFIR_AST_NODE_CLONE(mem, node->elseBranch);
        REQUIRE_ELSE(clone->elseBranch != NULL, {
            KEFIR_AST_NODE_FREE(mem, clone->thenBranch);
            KEFIR_AST_NODE_FREE(mem, clone->condition);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    } else {
        clone->elseBranch = NULL;
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_conditional_statement *kefir_ast_new_conditional_statement(struct kefir_mem *mem,
                                                                            struct kefir_ast_node_base *condition,
                                                                            struct kefir_ast_node_base *thenBranch,
                                                                            struct kefir_ast_node_base *elseBranch) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(condition != NULL, NULL);
    REQUIRE(thenBranch != NULL, NULL);

    struct kefir_ast_conditional_statement *stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_conditional_statement));
    REQUIRE(stmt != NULL, NULL);
    stmt->base.klass = &AST_CONDITIONAL_STATEMENT_CLASS;
    stmt->base.self = stmt;
    kefir_result_t res = kefir_ast_node_properties_init(&stmt->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, stmt);
        return NULL;
    });

    stmt->condition = condition;
    stmt->thenBranch = thenBranch;
    stmt->elseBranch = elseBranch;
    return stmt;
}
