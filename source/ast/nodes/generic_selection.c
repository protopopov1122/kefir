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

NODE_VISIT_IMPL(ast_generic_selection_visit, kefir_ast_generic_selection, generic_selection)

struct kefir_ast_node_base *ast_generic_selection_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_generic_selection_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_generic_selection *, node, base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->control));
    REQUIRE_OK(kefir_list_free(mem, &node->associations));
    if (node->default_assoc != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->default_assoc));
    }
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_GENERIC_SELECTION_CLASS = {.type = KEFIR_AST_GENERIC_SELECTION,
                                                                 .visit = ast_generic_selection_visit,
                                                                 .clone = ast_generic_selection_clone,
                                                                 .free = ast_generic_selection_free};

static kefir_result_t assoc_free(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                 void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(assoc->type_name)));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, assoc->expr));
    KEFIR_FREE(mem, assoc);
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_generic_selection_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_generic_selection *, node, base->self);
    struct kefir_ast_generic_selection *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_generic_selection));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_GENERIC_SELECTION_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->control = KEFIR_AST_NODE_CLONE(mem, node->control);
    REQUIRE_ELSE(clone->control != NULL, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_init(&clone->associations);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, clone->control);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_on_remove(&clone->associations, assoc_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &clone->associations);
        KEFIR_AST_NODE_FREE(mem, clone->control);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->associations); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc, iter->value);
        struct kefir_ast_generic_selection_assoc *clone_assoc =
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_generic_selection_assoc *));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_list_free(mem, &clone->associations);
            KEFIR_AST_NODE_FREE(mem, clone->control);
            KEFIR_FREE(mem, clone);
            return NULL;
        });

        struct kefir_ast_node_base *clone_type_name = KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(assoc->type_name));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_list_free(mem, &clone->associations);
            KEFIR_AST_NODE_FREE(mem, clone->control);
            KEFIR_FREE(mem, clone);
            return NULL;
        });

        clone_assoc->type_name = (struct kefir_ast_type_name *) clone_type_name->self;
        clone_assoc->expr = KEFIR_AST_NODE_CLONE(mem, assoc->expr);
        REQUIRE_ELSE(clone_assoc->expr != NULL, {
            KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(clone_assoc->type_name));
            KEFIR_FREE(mem, clone_assoc);
            kefir_list_free(mem, &clone->associations);
            KEFIR_AST_NODE_FREE(mem, clone->control);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
        res = kefir_list_insert_after(mem, &clone->associations, kefir_list_tail(&clone->associations), clone_assoc);
        REQUIRE_ELSE(clone_assoc->expr != NULL, {
            KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(clone_assoc->type_name));
            KEFIR_FREE(mem, clone_assoc);
            kefir_list_free(mem, &clone->associations);
            KEFIR_AST_NODE_FREE(mem, clone->control);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }
    if (node->default_assoc != NULL) {
        clone->default_assoc = KEFIR_AST_NODE_CLONE(mem, node->default_assoc);
        REQUIRE_ELSE(clone->default_assoc != NULL, {
            kefir_list_free(mem, &clone->associations);
            KEFIR_AST_NODE_FREE(mem, clone->control);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    } else {
        clone->default_assoc = NULL;
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_generic_selection *kefir_ast_new_generic_selection(struct kefir_mem *mem,
                                                                    struct kefir_ast_node_base *control) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(control != NULL, NULL);

    struct kefir_ast_generic_selection *selection = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_generic_selection));
    REQUIRE(selection != NULL, NULL);
    selection->base.klass = &AST_GENERIC_SELECTION_CLASS;
    selection->base.self = selection;
    kefir_result_t res = kefir_ast_node_properties_init(&selection->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, selection);
        return NULL;
    });
    res = kefir_source_location_empty(&selection->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, selection);
        return NULL;
    });
    selection->control = control;
    selection->default_assoc = NULL;
    res = kefir_list_init(&selection->associations);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, selection);
        return NULL;
    });
    res = kefir_list_on_remove(&selection->associations, assoc_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &selection->associations);
        KEFIR_FREE(mem, selection);
        return NULL;
    });
    return selection;
}

kefir_result_t kefir_ast_generic_selection_append(struct kefir_mem *mem, struct kefir_ast_generic_selection *selection,
                                                  struct kefir_ast_type_name *type_name,
                                                  struct kefir_ast_node_base *expr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(selection != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST generic selection"));
    REQUIRE(expr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST expression"));

    if (type_name != NULL) {
        struct kefir_ast_generic_selection_assoc *assoc =
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_generic_selection_assoc));
        REQUIRE(assoc != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST generic selection association"));
        assoc->type_name = type_name;
        assoc->expr = expr;
        kefir_result_t res =
            kefir_list_insert_after(mem, &selection->associations, kefir_list_tail(&selection->associations), assoc);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, assoc);
            return res;
        });
    } else {
        REQUIRE(
            selection->default_assoc == NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "AST generic selection cannot have multiple default associations"));
        selection->default_assoc = expr;
    }
    return KEFIR_OK;
}
