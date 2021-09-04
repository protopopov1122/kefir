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

#include "kefir/parser/scope.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

enum identifier_type { IDENTIFIER_TYPEDEF = 0, IDENTIFIER_VARIABLE };

kefir_result_t kefir_parser_block_scope_init(struct kefir_parser_block_scope *scope,
                                             struct kefir_symbol_table *symbols) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));

    scope->symbols = symbols;
    REQUIRE_OK(kefir_hashtree_init(&scope->identifier_declarations, &kefir_hashtree_str_ops));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_block_scope_free(struct kefir_mem *mem, struct kefir_parser_block_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));

    REQUIRE_OK(kefir_hashtree_free(mem, &scope->identifier_declarations));
    scope->symbols = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_block_scope_declare_typedef(struct kefir_mem *mem, struct kefir_parser_block_scope *scope,
                                                        const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));

    if (scope->symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, scope->symbols, identifier, NULL);
        REQUIRE(identifier != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to insert indentifier into symbol table"));
    }
    kefir_result_t res = kefir_hashtree_insert(mem, &scope->identifier_declarations, (kefir_hashtree_key_t) identifier,
                                               (kefir_hashtree_value_t) IDENTIFIER_TYPEDEF);
    if (res == KEFIR_ALREADY_EXISTS) {
        kefir_bool_t is_typedef;
        REQUIRE_OK(kefir_parser_block_scope_is_typedef(scope, identifier, &is_typedef));
        REQUIRE(is_typedef, KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Cannot redefine variable identifier as typedef"));
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_block_scope_declare_variable(struct kefir_mem *mem, struct kefir_parser_block_scope *scope,
                                                         const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));

    if (scope->symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, scope->symbols, identifier, NULL);
        REQUIRE(identifier != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to insert indentifier into symbol table"));
    }
    kefir_result_t res = kefir_hashtree_insert(mem, &scope->identifier_declarations, (kefir_hashtree_key_t) identifier,
                                               (kefir_hashtree_value_t) IDENTIFIER_VARIABLE);
    if (res == KEFIR_ALREADY_EXISTS) {
        kefir_bool_t is_typedef;
        REQUIRE_OK(kefir_parser_block_scope_is_typedef(scope, identifier, &is_typedef));
        REQUIRE(!is_typedef, KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Cannot redefine typedef as variable identifier"));
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_block_scope_is_typedef(struct kefir_parser_block_scope *scope, const char *identifier,
                                                   kefir_bool_t *result) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to result"));

    struct kefir_hashtree_node *node = NULL;
    REQUIRE_OK(kefir_hashtree_at(&scope->identifier_declarations, (kefir_hashtree_key_t) identifier, &node));
    *result = node->value == IDENTIFIER_TYPEDEF;
    return KEFIR_OK;
}

static kefir_result_t remove_block_scope(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                         void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_parser_block_scope *, scope, entry->value);
    REQUIRE_OK(kefir_parser_block_scope_free(mem, scope));
    KEFIR_FREE(mem, scope);
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scope_init(struct kefir_mem *mem, struct kefir_parser_scope *scope,
                                       struct kefir_symbol_table *symbols) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));

    scope->symbols = symbols;
    REQUIRE_OK(kefir_list_init(&scope->block_scopes));
    REQUIRE_OK(kefir_list_on_remove(&scope->block_scopes, remove_block_scope, NULL));
    REQUIRE_OK(kefir_parser_scope_push_block(mem, scope));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scope_free(struct kefir_mem *mem, struct kefir_parser_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));

    REQUIRE_OK(kefir_list_free(mem, &scope->block_scopes));
    scope->symbols = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scope_push_block(struct kefir_mem *mem, struct kefir_parser_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));

    struct kefir_parser_block_scope *block_scope = KEFIR_MALLOC(mem, sizeof(struct kefir_parser_block_scope));
    REQUIRE(block_scope != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST parser block scope"));
    kefir_result_t res = kefir_parser_block_scope_init(block_scope, scope->symbols);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, block_scope);
        return res;
    });
    res = kefir_list_insert_after(mem, &scope->block_scopes, kefir_list_tail(&scope->block_scopes), block_scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_parser_block_scope_free(mem, block_scope);
        KEFIR_FREE(mem, block_scope);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scope_pop_block(struct kefir_mem *mem, struct kefir_parser_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));

    REQUIRE(kefir_list_length(&scope->block_scopes) > 1,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot pop the base block of parser scope"));
    REQUIRE_OK(kefir_list_pop(mem, &scope->block_scopes, kefir_list_tail(&scope->block_scopes)));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scope_declare_typedef(struct kefir_mem *mem, struct kefir_parser_scope *scope,
                                                  const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));

    struct kefir_list_entry *tail = kefir_list_tail(&scope->block_scopes);
    REQUIRE(tail != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to retrieve current parser block scope"));
    ASSIGN_DECL_CAST(struct kefir_parser_block_scope *, block_scope, tail->value);
    REQUIRE_OK(kefir_parser_block_scope_declare_typedef(mem, block_scope, identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scope_declare_variable(struct kefir_mem *mem, struct kefir_parser_scope *scope,
                                                   const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser block scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));

    struct kefir_list_entry *tail = kefir_list_tail(&scope->block_scopes);
    REQUIRE(tail != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to retrieve current parser block scope"));
    ASSIGN_DECL_CAST(struct kefir_parser_block_scope *, block_scope, tail->value);
    REQUIRE_OK(kefir_parser_block_scope_declare_variable(mem, block_scope, identifier));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scope_is_typedef(struct kefir_parser_scope *scope, const char *identifier,
                                             kefir_bool_t *result) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to result"));

    kefir_bool_t found = false;
    const struct kefir_list_entry *iter = kefir_list_tail(&scope->block_scopes);
    for (; !found && iter != NULL; iter = iter->prev) {
        ASSIGN_DECL_CAST(struct kefir_parser_block_scope *, block_scope, iter->value);
        kefir_result_t res = kefir_parser_block_scope_is_typedef(block_scope, identifier, result);
        if (res == KEFIR_OK) {
            found = true;
        } else {
            REQUIRE(res == KEFIR_NOT_FOUND, res);
        }
    }
    REQUIRE(found, KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Cannot find requested identifier"));
    return KEFIR_OK;
}
