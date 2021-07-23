/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

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

NODE_VISIT_IMPL(ast_compound_statement_visit, kefir_ast_compound_statement, compound_statement)

struct kefir_ast_node_base *ast_compound_statement_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_compound_statement_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_compound_statement *, node, base->self);
    REQUIRE_OK(kefir_list_free(mem, &node->block_items));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_COMPOUND_STATEMENT_CLASS = {.type = KEFIR_AST_COMPOUND_STATEMENT,
                                                                  .visit = ast_compound_statement_visit,
                                                                  .clone = ast_compound_statement_clone,
                                                                  .free = ast_compound_statement_free};

kefir_result_t free_block_item(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                               void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item_base, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, item_base));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_compound_statement_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_compound_statement *, node, base->self);
    struct kefir_ast_compound_statement *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_compound_statement));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_COMPOUND_STATEMENT_CLASS;
    clone->base.self = clone;
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
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_compound_statement *kefir_ast_new_compound_statement(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_compound_statement *stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_compound_statement));
    REQUIRE(stmt != NULL, NULL);
    stmt->base.klass = &AST_COMPOUND_STATEMENT_CLASS;
    stmt->base.self = stmt;
    kefir_result_t res = kefir_ast_node_properties_init(&stmt->base.properties);
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
    return stmt;
}
