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

NODE_VISIT_IMPL(ast_conditional_operator_visit, kefir_ast_conditional_operator, conditional_operator)

struct kefir_ast_node_base *ast_conditional_operator_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_conditional_operator_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_conditional_operator *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->expr2));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->expr1));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->condition));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_CONDITIONAL_OPERATION_CLASS = {.type = KEFIR_AST_CONDITIONAL_OPERATION,
                                                                     .visit = ast_conditional_operator_visit,
                                                                     .clone = ast_conditional_operator_clone,
                                                                     .free = ast_conditional_operator_free};

struct kefir_ast_node_base *ast_conditional_operator_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_conditional_operator *, node, base->self);
    struct kefir_ast_conditional_operator *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_conditional_operator));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_CONDITIONAL_OPERATION_CLASS;
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
    clone->expr1 = KEFIR_AST_NODE_CLONE(mem, node->expr1);
    REQUIRE_ELSE(clone->expr1 != NULL, {
        KEFIR_AST_NODE_FREE(mem, clone->condition);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->expr2 = KEFIR_AST_NODE_CLONE(mem, node->expr2);
    REQUIRE_ELSE(clone->expr2 != NULL, {
        KEFIR_AST_NODE_FREE(mem, clone->expr1);
        KEFIR_AST_NODE_FREE(mem, clone->condition);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_conditional_operator *kefir_ast_new_conditional_operator(struct kefir_mem *mem,
                                                                          struct kefir_ast_node_base *condition,
                                                                          struct kefir_ast_node_base *expr1,
                                                                          struct kefir_ast_node_base *expr2) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(condition != NULL, NULL);
    REQUIRE(expr1 != NULL, NULL);
    REQUIRE(expr2 != NULL, NULL);
    struct kefir_ast_conditional_operator *oper = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_conditional_operator));
    REQUIRE(oper != NULL, NULL);
    oper->base.klass = &AST_CONDITIONAL_OPERATION_CLASS;
    oper->base.self = oper;
    kefir_result_t res = kefir_ast_node_properties_init(&oper->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, oper);
        return NULL;
    });
    oper->condition = condition;
    oper->expr1 = expr1;
    oper->expr2 = expr2;
    return oper;
}
