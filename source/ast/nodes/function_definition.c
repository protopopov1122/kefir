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
#include "kefir/ast/downcast.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_function_definition_visit, kefir_ast_function_definition, function_definition)

struct kefir_ast_node_base *ast_function_definition_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_function_definition_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_function_definition *, node, base->self);

    REQUIRE_OK(kefir_ast_declarator_specifier_list_free(mem, &node->specifiers));
    REQUIRE_OK(kefir_ast_declarator_free(mem, node->declarator));
    REQUIRE_OK(kefir_list_free(mem, &node->declarations));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(node->body)));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_FUNCTION_DEFINITION_CLASS = {.type = KEFIR_AST_FUNCTION_DEFINITION,
                                                                   .visit = ast_function_definition_visit,
                                                                   .clone = ast_function_definition_clone,
                                                                   .free = ast_function_definition_free};

static kefir_result_t declaration_entry_free(struct kefir_mem *mem, struct kefir_list *list,
                                             struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl, entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, decl));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_function_definition_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_function_definition *, node, base->self);
    struct kefir_ast_function_definition *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_function_definition));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_FUNCTION_DEFINITION_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_ast_declarator_specifier_list_clone(mem, &clone->specifiers, &node->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->declarator = kefir_ast_declarator_clone(mem, node->declarator);
    REQUIRE_ELSE(clone->declarator != NULL, {
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = kefir_list_init(&node->declarations);
    REQUIRE_CHAIN(&res, kefir_list_on_remove(&node->declarations, declaration_entry_free, NULL));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, clone->declarator);
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->declarations); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl, iter->value);

        struct kefir_ast_node_base *decl_clone = KEFIR_AST_NODE_CLONE(mem, decl);
        REQUIRE_ELSE(decl_clone != NULL, {
            kefir_list_free(mem, &clone->declarations);
            kefir_ast_declarator_free(mem, clone->declarator);
            kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
            KEFIR_FREE(mem, clone);
            return NULL;
        });

        res = kefir_list_insert_after(mem, &clone->declarations, kefir_list_tail(&clone->declarations), decl_clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, decl_clone);
            kefir_list_free(mem, &clone->declarations);
            kefir_ast_declarator_free(mem, clone->declarator);
            kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }

    struct kefir_ast_node_base *body_clone = KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(node->body));
    REQUIRE_ELSE(body_clone != NULL, {
        kefir_list_free(mem, &clone->declarations);
        kefir_ast_declarator_free(mem, clone->declarator);
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    REQUIRE_MATCH(&res, kefir_ast_downcast_compound_statement(body_clone, &clone->body),
                  KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unable to downcast function body to compound statement"));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, body_clone);
        kefir_list_free(mem, &clone->declarations);
        kefir_ast_declarator_free(mem, clone->declarator);
        kefir_ast_declarator_specifier_list_free(mem, &clone->specifiers);
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    return KEFIR_AST_NODE_BASE(clone);
}

static kefir_result_t insert_function_name_builtin(struct kefir_mem *mem, struct kefir_ast_declarator *declarator,
                                                   struct kefir_ast_compound_statement *body) {
    const char *function_identifier = NULL;
    REQUIRE_OK(kefir_ast_declarator_unpack_identifier(declarator, &function_identifier));

    struct kefir_ast_declarator *func_name_id_declarator = kefir_ast_declarator_identifier(mem, NULL, "__func__");
    REQUIRE(func_name_id_declarator != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate __func__ declarator"));
    struct kefir_ast_declarator *func_name_declarator =
        kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED, NULL, func_name_id_declarator);
    REQUIRE_ELSE(func_name_declarator != NULL, {
        kefir_ast_declarator_free(mem, func_name_id_declarator);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate __func__ declarator");
    });

    struct kefir_ast_string_literal *func_name_value =
        kefir_ast_new_string_literal_multibyte(mem, function_identifier, strlen(function_identifier) + 1);
    REQUIRE_ELSE(func_name_value != NULL, {
        kefir_ast_declarator_free(mem, func_name_declarator);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate __func__ string literal");
    });

    struct kefir_ast_initializer *func_name_initializer =
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(func_name_value));
    REQUIRE_ELSE(func_name_initializer != NULL, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(func_name_value));
        kefir_ast_declarator_free(mem, func_name_declarator);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate __func__ initializer");
    });

    struct kefir_ast_declaration *func_name_declaration =
        kefir_ast_new_single_declaration(mem, func_name_declarator, func_name_initializer, NULL);
    REQUIRE_ELSE(func_name_declaration != NULL, {
        kefir_ast_initializer_free(mem, func_name_initializer);
        kefir_ast_declarator_free(mem, func_name_declarator);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Faile to allocate __func__ declaration");
    });

#define APPEND_SPECIFIER(_spec)                                                                                \
    do {                                                                                                       \
        struct kefir_ast_declarator_specifier *specifier1 = (_spec);                                           \
        REQUIRE_ELSE(specifier1 != NULL, {                                                                     \
            KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(func_name_declaration));                              \
            return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Faile to allocate __func__ declarator specifier"); \
        });                                                                                                    \
        kefir_result_t res =                                                                                   \
            kefir_ast_declarator_specifier_list_append(mem, &func_name_declaration->specifiers, specifier1);   \
        REQUIRE_ELSE(res == KEFIR_OK, {                                                                        \
            kefir_ast_declarator_specifier_free(mem, specifier1);                                              \
            KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(func_name_declaration));                              \
            return res;                                                                                        \
        });                                                                                                    \
    } while (0)

    APPEND_SPECIFIER(kefir_ast_storage_class_specifier_static(mem));
    APPEND_SPECIFIER(kefir_ast_type_qualifier_const(mem));
    APPEND_SPECIFIER(kefir_ast_type_specifier_char(mem));
#undef APPEND_SPECIFIER

    kefir_result_t res =
        kefir_list_insert_after(mem, &body->block_items, NULL, KEFIR_AST_NODE_BASE(func_name_declaration));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(func_name_declaration));
        return res;
    });
    return KEFIR_OK;
}

struct kefir_ast_function_definition *kefir_ast_new_function_definition(struct kefir_mem *mem,
                                                                        struct kefir_ast_declarator *declarator,
                                                                        struct kefir_ast_compound_statement *body) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(declarator != NULL, NULL);
    REQUIRE(body != NULL, NULL);

    kefir_result_t res = insert_function_name_builtin(mem, declarator, body);
    REQUIRE(res == KEFIR_OK, NULL);

    struct kefir_ast_function_definition *func = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_function_definition));
    REQUIRE(func != NULL, NULL);
    func->base.klass = &AST_FUNCTION_DEFINITION_CLASS;
    func->base.self = func;
    res = kefir_ast_node_properties_init(&func->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, func);
        return NULL;
    });
    res = kefir_source_location_empty(&func->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, func);
        return NULL;
    });

    func->declarator = declarator;
    func->body = body;

    res = kefir_ast_declarator_specifier_list_init(&func->specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, func);
        return NULL;
    });

    res = kefir_list_init(&func->declarations);
    REQUIRE_CHAIN(&res, kefir_list_on_remove(&func->declarations, declaration_entry_free, NULL));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &func->specifiers);
        KEFIR_FREE(mem, func);
        return NULL;
    });
    return func;
}
