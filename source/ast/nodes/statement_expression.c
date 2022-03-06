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

NODE_VISIT_IMPL(ast_statement_expression_visit, kefir_ast_statement_expression, statement_expression)

struct kefir_ast_node_base *ast_statement_expression_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_statement_expression_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_statement_expression *, node, base->self);
    REQUIRE_OK(kefir_list_free(mem, &node->block_items));
    if (node->result != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->result));
        node->result = NULL;
    }
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_STATEMENT_EXPRESSION_CLASS = {.type = KEFIR_AST_STATEMENT_EXPRESSION,
                                                                    .visit = ast_statement_expression_visit,
                                                                    .clone = ast_statement_expression_clone,
                                                                    .free = ast_statement_expression_free};

static kefir_result_t free_block_item(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                      void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item_base, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, item_base));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_statement_expression_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_statement_expression *, node, base->self);
    struct kefir_ast_statement_expression *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_statement_expression));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_STATEMENT_EXPRESSION_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_list_init(&clone->block_items);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_list_on_remove(&clone->block_items, free_block_item, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &clone->block_items);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    for (const struct kefir_list_entry *iter = kefir_list_head(&clone->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);
        struct kefir_ast_node_base *item_clone = KEFIR_AST_NODE_CLONE(mem, item);
        REQUIRE_ELSE(item_clone != NULL, {
            kefir_list_free(mem, &clone->block_items);
            KEFIR_FREE(mem, clone);
            return NULL;
        });

        res = kefir_list_insert_after(mem, &clone->block_items, kefir_list_tail(&clone->block_items), item_clone);
        REQUIRE_ELSE(item_clone != NULL, {
            KEFIR_AST_NODE_FREE(mem, item_clone);
            kefir_list_free(mem, &clone->block_items);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }

    if (node->result != NULL) {
        clone->result = KEFIR_AST_NODE_CLONE(mem, node->result);
        REQUIRE_ELSE(clone->result != NULL, {
            kefir_list_free(mem, &clone->block_items);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    } else {
        clone->result = NULL;
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_statement_expression *kefir_ast_new_statement_expression(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_statement_expression *stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_statement_expression));
    REQUIRE(stmt != NULL, NULL);
    stmt->base.klass = &AST_STATEMENT_EXPRESSION_CLASS;
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

    res = kefir_list_init(&stmt->block_items);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, stmt);
        return NULL;
    });

    res = kefir_list_on_remove(&stmt->block_items, free_block_item, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, stmt);
        return NULL;
    });
    stmt->result = NULL;
    return stmt;
}
