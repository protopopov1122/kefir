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

NODE_VISIT_IMPL(ast_assignment_operator_visit, kefir_ast_assignment_operator, assignment_operator)

struct kefir_ast_node_base *ast_assignment_operator_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_assignment_operator_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_assignment_operator *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->target));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->value));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_ASSIGNMENT_OPERATOR_CLASS = {.type = KEFIR_AST_ASSIGNMENT_OPERATOR,
                                                                   .visit = ast_assignment_operator_visit,
                                                                   .clone = ast_assignment_operator_clone,
                                                                   .free = ast_assignment_operator_free};

struct kefir_ast_node_base *ast_assignment_operator_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_assignment_operator *, node, base->self);
    struct kefir_ast_assignment_operator *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_assignment_operator));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_ASSIGNMENT_OPERATOR_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->operation = node->operation;
    clone->target = KEFIR_AST_NODE_CLONE(mem, node->target);
    REQUIRE_ELSE(clone->target != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->value = KEFIR_AST_NODE_CLONE(mem, node->value);
    REQUIRE_ELSE(clone->value != NULL, {
        KEFIR_AST_NODE_FREE(mem, clone->target);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_assignment_operator *kefir_ast_new_simple_assignment(struct kefir_mem *mem,
                                                                      struct kefir_ast_node_base *target,
                                                                      struct kefir_ast_node_base *value) {
    return kefir_ast_new_compound_assignment(mem, KEFIR_AST_ASSIGNMENT_SIMPLE, target, value);
}

struct kefir_ast_assignment_operator *kefir_ast_new_compound_assignment(struct kefir_mem *mem,
                                                                        kefir_ast_assignment_operation_t oper,
                                                                        struct kefir_ast_node_base *target,
                                                                        struct kefir_ast_node_base *value) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(target != NULL, NULL);
    REQUIRE(value != NULL, NULL);

    struct kefir_ast_assignment_operator *assignment = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_assignment_operator));
    REQUIRE(assignment != NULL, NULL);
    assignment->base.klass = &AST_ASSIGNMENT_OPERATOR_CLASS;
    assignment->base.self = assignment;
    kefir_result_t res = kefir_ast_node_properties_init(&assignment->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, assignment);
        return NULL;
    });
    assignment->operation = oper;
    assignment->target = target;
    assignment->value = value;
    return assignment;
}
