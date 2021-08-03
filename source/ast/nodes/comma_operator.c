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

NODE_VISIT_IMPL(ast_comma_operator_visit, kefir_ast_comma_operator, comma_operator)

struct kefir_ast_node_base *ast_comma_operator_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_comma_operator_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_comma_operator *, node, base->self);
    REQUIRE_OK(kefir_list_free(mem, &node->expressions));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_COMMA_OPERATOR_CLASS = {.type = KEFIR_AST_COMMA_OPERATOR,
                                                              .visit = ast_comma_operator_visit,
                                                              .clone = ast_comma_operator_clone,
                                                              .free = ast_comma_operator_free};

static kefir_result_t comma_on_removal(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                       void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_comma_operator_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_comma_operator *, node, base->self);
    struct kefir_ast_comma_operator *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_comma_operator));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_COMMA_OPERATOR_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_list_init(&clone->expressions);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_on_remove(&clone->expressions, comma_on_removal, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &clone->expressions);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->expressions); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, expr, iter->value);

        struct kefir_ast_node_base *expr_clone = KEFIR_AST_NODE_CLONE(mem, expr);
        REQUIRE_ELSE(expr_clone != NULL, {
            kefir_list_free(mem, &clone->expressions);
            KEFIR_FREE(mem, clone);
            return NULL;
        });

        res = kefir_list_insert_after(mem, &clone->expressions, kefir_list_tail(&clone->expressions), expr_clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, expr_clone);
            kefir_list_free(mem, &clone->expressions);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_comma_operator *kefir_ast_new_comma_operator(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_comma_operator *comma = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_comma_operator));
    REQUIRE(comma != NULL, NULL);
    comma->base.klass = &AST_COMMA_OPERATOR_CLASS;
    comma->base.self = comma;
    kefir_result_t res = kefir_ast_node_properties_init(&comma->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, comma);
        return NULL;
    });
    res = kefir_list_init(&comma->expressions);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, comma);
        return NULL;
    });
    res = kefir_list_on_remove(&comma->expressions, comma_on_removal, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &comma->expressions);
        KEFIR_FREE(mem, comma);
        return NULL;
    });
    return comma;
}

kefir_result_t kefir_ast_comma_append(struct kefir_mem *mem, struct kefir_ast_comma_operator *comma,
                                      struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(comma != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST comma operator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node"));

    REQUIRE_OK(kefir_list_insert_after(mem, &comma->expressions, kefir_list_tail(&comma->expressions), base));
    return KEFIR_OK;
}
