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

NODE_VISIT_IMPL(ast_declaration_visit, kefir_ast_declaration, declaration)

struct kefir_ast_node_base *ast_declaration_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_declaration_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, node, base->self);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_free(mem, &node->specifiers));
    REQUIRE_OK(kefir_list_free(mem, &node->init_declarators));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_DECLARATION_LIST_CLASS = {.type = KEFIR_AST_DECLARATION,
                                                                .visit = ast_declaration_visit,
                                                                .clone = ast_declaration_clone,
                                                                .free = ast_declaration_free};

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

struct kefir_ast_node_base *ast_declaration_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, node, base->self);
    struct kefir_ast_declaration *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declaration));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_DECLARATION_LIST_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
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

    res = kefir_list_init(&clone->init_declarators);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_on_remove(&clone->init_declarators, declaration_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &clone->init_declarators);
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->init_declarators); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, declaration, iter->value);
        struct kefir_ast_node_base *declaration_clone = KEFIR_AST_NODE_CLONE(mem, declaration);
        REQUIRE_ELSE(declaration_clone != NULL, {
            kefir_list_free(mem, &clone->init_declarators);
            kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
        res = kefir_list_insert_after(mem, &clone->init_declarators, kefir_list_tail(&clone->init_declarators),
                                      declaration_clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_list_free(mem, &clone->init_declarators);
            kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }

    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_declaration *kefir_ast_new_declaration(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_declaration *declaration = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declaration));
    REQUIRE(declaration != NULL, NULL);
    declaration->base.klass = &AST_DECLARATION_LIST_CLASS;
    declaration->base.self = declaration;
    kefir_result_t res = kefir_ast_node_properties_init(&declaration->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, declaration);
        return NULL;
    });
    res = kefir_source_location_empty(&declaration->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, declaration);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_init(&declaration->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, declaration);
        return NULL;
    });

    REQUIRE_CHAIN(&res, kefir_list_init(&declaration->init_declarators));
    REQUIRE_CHAIN(&res, kefir_list_on_remove(&declaration->init_declarators, declaration_free, NULL));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &declaration->specifiers);
        KEFIR_FREE(mem, declaration);
        return NULL;
    });
    return declaration;
}

struct kefir_ast_declaration *kefir_ast_new_single_declaration(struct kefir_mem *mem,
                                                               struct kefir_ast_declarator *declarator,
                                                               struct kefir_ast_initializer *initializer,
                                                               struct kefir_ast_init_declarator **declaration_ptr) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_declaration *decl_list = kefir_ast_new_declaration(mem);
    REQUIRE(decl_list != NULL, NULL);
    struct kefir_ast_init_declarator *declaration =
        kefir_ast_new_init_declarator(mem, decl_list, declarator, initializer);
    REQUIRE_ELSE(declaration != NULL, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl_list));
        return NULL;
    });
    kefir_result_t res =
        kefir_list_insert_after(mem, &decl_list->init_declarators, kefir_list_tail(&decl_list->init_declarators),
                                KEFIR_AST_NODE_BASE(declaration));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(decl_list));
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(declaration));
        return NULL;
    });
    ASSIGN_PTR(declaration_ptr, declaration);
    return decl_list;
}

kefir_result_t kefir_ast_declaration_unpack_single(struct kefir_ast_declaration *list,
                                                   struct kefir_ast_init_declarator **declaration_ptr) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declaration list"));
    REQUIRE(declaration_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST declaration"));

    REQUIRE(kefir_list_length(&list->init_declarators) == 1,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected declaration list to contain a single declaration"));
    struct kefir_ast_node_base *node = kefir_list_head(&list->init_declarators)->value;
    REQUIRE(node->klass->type == KEFIR_AST_INIT_DECLARATOR,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected declaration list to contain a single declaration"));
    *declaration_ptr = node->self;
    return KEFIR_OK;
}
