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

// TODO Implement declaration list specific tests

NODE_VISIT_IMPL(ast_declaration_list_visit, kefir_ast_declaration_list, declaration_list)

struct kefir_ast_node_base *ast_declaration_list_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_declaration_list_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_declaration_list *, node, base->self);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_free(mem, &node->specifiers));
    REQUIRE_OK(kefir_list_free(mem, &node->declarations));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_DECLARATION_LIST_CLASS = {.type = KEFIR_AST_DECLARATION_LIST,
                                                                .visit = ast_declaration_list_visit,
                                                                .clone = ast_declaration_list_clone,
                                                                .free = ast_declaration_list_free};

static kefir_result_t declaration_free(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                       void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, declaration, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, declaration));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_declaration_list_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_declaration_list *, node, base->self);
    struct kefir_ast_declaration_list *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declaration_list));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_DECLARATION_LIST_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_init(&clone->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_clone(mem, &clone->specifiers, &node->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_list_init(&clone->declarations);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_on_remove(&clone->declarations, declaration_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &clone->declarations);
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->declarations); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, declaration, iter->value);
        struct kefir_ast_node_base *declaration_clone = KEFIR_AST_NODE_CLONE(mem, declaration);
        REQUIRE_ELSE(declaration_clone != NULL, {
            kefir_list_free(mem, &clone->declarations);
            kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
        res = kefir_list_insert_after(mem, &clone->declarations, kefir_list_tail(&clone->declarations),
                                      declaration_clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_list_free(mem, &clone->declarations);
            kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }

    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_declaration_list *kefir_ast_new_declaration_list(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_declaration_list *declaration_list = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declaration_list));
    REQUIRE(declaration_list != NULL, NULL);
    declaration_list->base.klass = &AST_DECLARATION_LIST_CLASS;
    declaration_list->base.self = declaration_list;
    kefir_result_t res = kefir_ast_node_properties_init(&declaration_list->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, declaration_list);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_init(&declaration_list->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, declaration_list);
        return NULL;
    });

    REQUIRE_CHAIN(&res, kefir_list_init(&declaration_list->declarations));
    REQUIRE_CHAIN(&res, kefir_list_on_remove(&declaration_list->declarations, declaration_free, NULL));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &declaration_list->specifiers);
        KEFIR_FREE(mem, declaration_list);
        return NULL;
    });
    return declaration_list;
}

struct kefir_ast_declaration_list *kefir_ast_new_single_declaration_list(
    struct kefir_mem *mem, struct kefir_ast_declarator *declarator, struct kefir_ast_initializer *initializer,
    struct kefir_ast_declaration **declaration_ptr) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_declaration_list *decl_list = kefir_ast_new_declaration_list(mem);
    REQUIRE(decl_list != NULL, NULL);
    struct kefir_ast_declaration *declaration = kefir_ast_new_declaration(mem, decl_list, declarator, initializer);
    REQUIRE_ELSE(declaration != NULL, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl_list));
        return NULL;
    });
    kefir_result_t res = kefir_list_insert_after(
        mem, &decl_list->declarations, kefir_list_tail(&decl_list->declarations), KEFIR_AST_NODE_BASE(declaration));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl_list));
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(declaration));
        return NULL;
    });
    ASSIGN_PTR(declaration_ptr, declaration);
    return decl_list;
}
