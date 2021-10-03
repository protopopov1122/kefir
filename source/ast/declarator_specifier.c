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

#include "kefir/ast/declarator.h"
#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t remove_declarator_specifier(struct kefir_mem *mem, struct kefir_list *list,
                                                  struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier *, specifier, entry->value);
    REQUIRE_OK(kefir_ast_declarator_specifier_free(mem, specifier));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_specifier_list_init(struct kefir_ast_declarator_specifier_list *list) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier list"));

    REQUIRE_OK(kefir_list_init(&list->list));
    REQUIRE_OK(kefir_list_on_remove(&list->list, remove_declarator_specifier, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_specifier_list_free(struct kefir_mem *mem,
                                                        struct kefir_ast_declarator_specifier_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier list"));

    REQUIRE_OK(kefir_list_free(mem, &list->list));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_specifier_list_append(struct kefir_mem *mem,
                                                          struct kefir_ast_declarator_specifier_list *list,
                                                          struct kefir_ast_declarator_specifier *specifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier list"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier"));

    REQUIRE_OK(kefir_list_insert_after(mem, &list->list, kefir_list_tail(&list->list), specifier));
    return KEFIR_OK;
}

struct kefir_list_entry *kefir_ast_declarator_specifier_list_iter(
    const struct kefir_ast_declarator_specifier_list *list, struct kefir_ast_declarator_specifier **specifier_ptr) {
    REQUIRE(list != NULL, NULL);

    struct kefir_list_entry *iter = kefir_list_head(&list->list);
    if (iter != NULL) {
        ASSIGN_PTR(specifier_ptr, (struct kefir_ast_declarator_specifier *) iter->value);
    }
    return iter;
}

kefir_result_t kefir_ast_declarator_specifier_list_next(struct kefir_list_entry **iter,
                                                        struct kefir_ast_declarator_specifier **specifier_ptr) {
    REQUIRE(iter != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier list iterator"));
    REQUIRE(*iter != NULL, KEFIR_OK);

    *iter = (*iter)->next;
    if (*iter != NULL) {
        ASSIGN_PTR(specifier_ptr, (struct kefir_ast_declarator_specifier *) (*iter)->value);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_specifier_list_remove(struct kefir_mem *mem,
                                                          struct kefir_ast_declarator_specifier_list *list,
                                                          struct kefir_list_entry *iter) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier list"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list iterator"));

    REQUIRE_OK(kefir_list_pop(mem, &list->list, iter));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_specifier_list_clone(struct kefir_mem *mem,
                                                         struct kefir_ast_declarator_specifier_list *dst,
                                                         const struct kefir_ast_declarator_specifier_list *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(dst != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination AST declarator specifier list"));
    REQUIRE(src != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source AST declarator specifier list"));

    struct kefir_ast_declarator_specifier *specifier = NULL;
    for (struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(src, &specifier); iter != NULL;
         kefir_ast_declarator_specifier_list_next(&iter, &specifier)) {
        struct kefir_ast_declarator_specifier *clone = kefir_ast_declarator_specifier_clone(mem, specifier);
        REQUIRE(clone != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to clone AST declarator specifier"));

        kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, dst, clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_declarator_specifier_free(mem, clone);
            return res;
        });
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_specifier_list_move_all(struct kefir_ast_declarator_specifier_list *dst,
                                                            struct kefir_ast_declarator_specifier_list *src) {
    REQUIRE(dst != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination AST declarator specifier list"));
    REQUIRE(src != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source AST declarator specifier list"));

    REQUIRE_OK(kefir_list_move_all(&dst->list, &src->list));
    return KEFIR_OK;
}

const struct kefir_source_location *kefir_ast_declarator_specifier_list_source_location(
    const struct kefir_ast_declarator_specifier_list *list) {
    REQUIRE(list != NULL, NULL);
    struct kefir_ast_declarator_specifier *specifier = NULL;
    const struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(list, &specifier);
    REQUIRE(iter != NULL && specifier != NULL, NULL);
    return &specifier->source_location;
}

static kefir_result_t struct_entry_remove(struct kefir_mem *mem, struct kefir_list *list,
                                          struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, decl_entry, entry->value);
    REQUIRE_OK(kefir_ast_structure_declaration_entry_free(mem, decl_entry));
    return KEFIR_OK;
}

struct kefir_ast_structure_specifier *kefir_ast_structure_specifier_init(struct kefir_mem *mem,
                                                                         struct kefir_symbol_table *symbols,
                                                                         const char *identifier,
                                                                         kefir_bool_t complete) {
    REQUIRE(mem != NULL, NULL);

    if (symbols != NULL && identifier != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_ast_structure_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_structure_specifier));
    REQUIRE(specifier != NULL, NULL);

    if (complete) {
        REQUIRE(kefir_list_init(&specifier->entries) == KEFIR_OK, NULL);
        REQUIRE(kefir_list_on_remove(&specifier->entries, struct_entry_remove, NULL) == KEFIR_OK, NULL);
    }

    specifier->identifier = identifier;
    specifier->complete = complete;
    return specifier;
}

kefir_result_t kefir_ast_structure_specifier_free(struct kefir_mem *mem,
                                                  struct kefir_ast_structure_specifier *specifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST structure specifier"));

    if (specifier->complete) {
        REQUIRE_OK(kefir_list_free(mem, &specifier->entries));
    }
    specifier->complete = false;
    specifier->identifier = NULL;
    KEFIR_FREE(mem, specifier);
    return KEFIR_OK;
}

struct kefir_ast_structure_specifier *kefir_ast_structure_specifier_clone(
    struct kefir_mem *mem, const struct kefir_ast_structure_specifier *specifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(specifier != NULL, NULL);

    struct kefir_ast_structure_specifier *clone =
        kefir_ast_structure_specifier_init(mem, NULL, specifier->identifier, specifier->complete);
    REQUIRE(clone != NULL, NULL);
    if (clone->complete) {
        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->entries); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, iter->value);

            struct kefir_ast_structure_declaration_entry *entry_clone = NULL;
            if (entry->is_static_assertion) {
                struct kefir_ast_node_base *assertion_clone =
                    KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(entry->static_assertion));
                REQUIRE_ELSE(assertion_clone != NULL, {
                    kefir_ast_structure_specifier_free(mem, clone);
                    return NULL;
                });
                entry_clone = kefir_ast_structure_declaration_entry_alloc_assert(
                    mem, (struct kefir_ast_static_assertion *) assertion_clone->self);
                REQUIRE_ELSE(entry_clone != NULL, {
                    KEFIR_AST_NODE_FREE(mem, assertion_clone);
                    kefir_ast_structure_specifier_free(mem, clone);
                    return NULL;
                });
            } else {
                entry_clone = kefir_ast_structure_declaration_entry_alloc(mem);

                struct kefir_ast_declarator_specifier *entry_specifier = NULL;
                for (struct kefir_list_entry *iter =
                         kefir_ast_declarator_specifier_list_iter(&entry->declaration.specifiers, &entry_specifier);
                     iter != NULL; kefir_ast_declarator_specifier_list_next(&iter, &entry_specifier)) {
                    struct kefir_ast_declarator_specifier *entry_specifier_clone =
                        kefir_ast_declarator_specifier_clone(mem, entry_specifier);
                    REQUIRE_ELSE(entry_specifier_clone != NULL, {
                        kefir_ast_structure_declaration_entry_free(mem, entry_clone);
                        kefir_ast_structure_specifier_free(mem, clone);
                        return NULL;
                    });
                    kefir_result_t res = kefir_ast_declarator_specifier_list_append(
                        mem, &entry_clone->declaration.specifiers, entry_specifier_clone);
                    REQUIRE_ELSE(res == KEFIR_OK, {
                        kefir_ast_declarator_specifier_free(mem, entry_specifier_clone);
                        kefir_ast_structure_declaration_entry_free(mem, entry_clone);
                        kefir_ast_structure_specifier_free(mem, clone);
                        return NULL;
                    });
                }

                for (const struct kefir_list_entry *iter = kefir_list_head(&entry->declaration.declarators);
                     iter != NULL; kefir_list_next(&iter)) {
                    ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, entry_declarator, iter->value);
                    struct kefir_ast_declarator *declarator_clone =
                        kefir_ast_declarator_clone(mem, entry_declarator->declarator);
                    struct kefir_ast_node_base *bitwidth_clone = KEFIR_AST_NODE_CLONE(mem, entry_declarator->bitwidth);
                    REQUIRE_ELSE(entry_declarator->bitwidth == NULL || bitwidth_clone != NULL, {
                        kefir_ast_declarator_free(mem, declarator_clone);
                        kefir_ast_structure_declaration_entry_free(mem, entry_clone);
                        kefir_ast_structure_specifier_free(mem, clone);
                        return NULL;
                    });
                    kefir_result_t res = kefir_ast_structure_declaration_entry_append(mem, entry_clone,
                                                                                      declarator_clone, bitwidth_clone);
                    REQUIRE_ELSE(res == KEFIR_OK, {
                        kefir_ast_declarator_free(mem, declarator_clone);
                        KEFIR_AST_NODE_FREE(mem, bitwidth_clone);
                        kefir_ast_structure_declaration_entry_free(mem, entry_clone);
                        kefir_ast_structure_specifier_free(mem, clone);
                        return NULL;
                    });
                }
            }

            kefir_result_t res = kefir_ast_structure_specifier_append_entry(mem, clone, entry_clone);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_structure_declaration_entry_free(mem, entry_clone);
                kefir_ast_structure_specifier_free(mem, clone);
                return NULL;
            });
        }
    }
    return clone;
}

kefir_result_t kefir_ast_structure_specifier_append_entry(struct kefir_mem *mem,
                                                          struct kefir_ast_structure_specifier *specifier,
                                                          struct kefir_ast_structure_declaration_entry *entry) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST structure specifier"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST structure declaration entry"));
    REQUIRE(specifier->complete,
            KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Unable to insert into incomplete structure declaration"));

    REQUIRE_OK(kefir_list_insert_after(mem, &specifier->entries, kefir_list_tail(&specifier->entries), entry));
    return KEFIR_OK;
}

static kefir_result_t declaration_declarator_remove(struct kefir_mem *mem, struct kefir_list *list,
                                                    struct kefir_list_entry *entry, void *payload) {
    UNUSED(list != NULL);
    UNUSED(payload != NULL);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, declarator, entry->value);
    REQUIRE_OK(kefir_ast_declarator_free(mem, declarator->declarator));
    declarator->declarator = NULL;
    if (declarator->bitwidth != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, declarator->bitwidth));
        declarator->bitwidth = NULL;
    }
    KEFIR_FREE(mem, declarator);
    return KEFIR_OK;
}

struct kefir_ast_structure_declaration_entry *kefir_ast_structure_declaration_entry_alloc(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_structure_declaration_entry *entry =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_structure_declaration_entry));
    REQUIRE(entry != NULL, NULL);

    entry->is_static_assertion = false;
    kefir_result_t res = kefir_ast_declarator_specifier_list_init(&entry->declaration.specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, entry);
        return NULL;
    });

    res = kefir_list_init(&entry->declaration.declarators);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &entry->declaration.specifiers);
        KEFIR_FREE(mem, entry);
        return NULL;
    });

    res = kefir_list_on_remove(&entry->declaration.declarators, declaration_declarator_remove, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &entry->declaration.declarators);
        kefir_ast_declarator_specifier_list_free(mem, &entry->declaration.specifiers);
        KEFIR_FREE(mem, entry);
        return NULL;
    });

    return entry;
}

struct kefir_ast_structure_declaration_entry *kefir_ast_structure_declaration_entry_alloc_assert(
    struct kefir_mem *mem, struct kefir_ast_static_assertion *static_assertion) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(static_assertion != NULL, NULL);

    struct kefir_ast_structure_declaration_entry *entry =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_structure_declaration_entry));
    REQUIRE(entry != NULL, NULL);

    entry->is_static_assertion = true;
    entry->static_assertion = static_assertion;
    return entry;
}

kefir_result_t kefir_ast_structure_declaration_entry_free(struct kefir_mem *mem,
                                                          struct kefir_ast_structure_declaration_entry *entry) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST structure declaration entry"));

    if (entry->is_static_assertion) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(entry->static_assertion)));
        entry->static_assertion = NULL;
        entry->is_static_assertion = false;
    } else {
        REQUIRE_OK(kefir_ast_declarator_specifier_list_free(mem, &entry->declaration.specifiers));
        REQUIRE_OK(kefir_list_free(mem, &entry->declaration.declarators));
    }
    KEFIR_FREE(mem, entry);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_structure_declaration_entry_append(struct kefir_mem *mem,
                                                            struct kefir_ast_structure_declaration_entry *entry,
                                                            struct kefir_ast_declarator *declarator,
                                                            struct kefir_ast_node_base *bitwidth) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST structure declaration entry"));
    REQUIRE(declarator != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator"));
    REQUIRE(!entry->is_static_assertion,
            KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Unable to append declarators to a static assertion entry"));

    struct kefir_ast_structure_entry_declarator *entry_declarator =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_structure_entry_declarator));
    REQUIRE(entry_declarator != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST structure entry declarator"));
    entry_declarator->declarator = declarator;
    entry_declarator->bitwidth = bitwidth;

    kefir_result_t res = kefir_list_insert_after(mem, &entry->declaration.declarators,
                                                 kefir_list_tail(&entry->declaration.declarators), entry_declarator);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, entry_declarator);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t remove_enum_entry(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                        void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, enum_entry, entry->value);
    if (enum_entry->value != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, enum_entry->value));
    }
    enum_entry->value = NULL;
    enum_entry->constant = NULL;
    KEFIR_FREE(mem, enum_entry);
    return KEFIR_OK;
}

struct kefir_ast_enum_specifier *kefir_ast_enum_specifier_init(struct kefir_mem *mem,
                                                               struct kefir_symbol_table *symbols,
                                                               const char *identifier, kefir_bool_t complete) {
    REQUIRE(mem != NULL, NULL);

    if (symbols != NULL && identifier != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_ast_enum_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_enum_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->identifier = identifier;
    specifier->complete = complete;
    if (complete) {
        REQUIRE(kefir_list_init(&specifier->entries) == KEFIR_OK, NULL);
        REQUIRE(kefir_list_on_remove(&specifier->entries, remove_enum_entry, NULL) == KEFIR_OK, NULL);
    }

    return specifier;
}

kefir_result_t kefir_ast_enum_specifier_free(struct kefir_mem *mem, struct kefir_ast_enum_specifier *specifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST enum specifier"));

    if (specifier->complete) {
        REQUIRE_OK(kefir_list_free(mem, &specifier->entries));
    }
    specifier->complete = false;
    specifier->identifier = NULL;
    KEFIR_FREE(mem, specifier);
    return KEFIR_OK;
}

struct kefir_ast_enum_specifier *kefir_ast_enum_specifier_clone(struct kefir_mem *mem,
                                                                const struct kefir_ast_enum_specifier *specifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(specifier != NULL, NULL);

    struct kefir_ast_enum_specifier *clone =
        kefir_ast_enum_specifier_init(mem, NULL, specifier->identifier, specifier->complete);
    REQUIRE(clone != NULL, NULL);
    if (clone->complete) {
        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->entries); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
            struct kefir_ast_node_base *value = KEFIR_AST_NODE_CLONE(mem, entry->value);
            REQUIRE_ELSE(entry->value == NULL || value != NULL, {
                kefir_ast_enum_specifier_free(mem, clone);
                return NULL;
            });

            kefir_result_t res = kefir_ast_enum_specifier_append(mem, clone, NULL, entry->constant, value);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_NODE_FREE(mem, value);
                kefir_ast_enum_specifier_free(mem, clone);
                return NULL;
            });
        }
    }
    return clone;
}

kefir_result_t kefir_ast_enum_specifier_append(struct kefir_mem *mem, struct kefir_ast_enum_specifier *specifier,
                                               struct kefir_symbol_table *symbols, const char *identifier,
                                               struct kefir_ast_node_base *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST enum specifier"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST enum entry identifier"));
    REQUIRE(specifier->complete,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected AST enum specifier to be complete"));

    if (symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to insert identifier into symbol table"));
    }

    struct kefir_ast_enum_specifier_entry *entry = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_enum_specifier_entry));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST enum specifier entry"));
    entry->constant = identifier;
    entry->value = value;

    kefir_result_t res = kefir_list_insert_after(mem, &specifier->entries, kefir_list_tail(&specifier->entries), entry);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, entry);
        return res;
    });
    return KEFIR_OK;
}

#define TYPE_SPECIFIER(_id, _spec)                                                                 \
    struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_##_id(struct kefir_mem *mem) { \
        REQUIRE(mem != NULL, NULL);                                                                \
        struct kefir_ast_declarator_specifier *specifier =                                         \
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));                      \
        REQUIRE(specifier != NULL, NULL);                                                          \
                                                                                                   \
        specifier->klass = KEFIR_AST_TYPE_SPECIFIER;                                               \
        specifier->type_specifier.specifier = (_spec);                                             \
        kefir_result_t res = kefir_source_location_empty(&specifier->source_location);             \
        REQUIRE_ELSE(res == KEFIR_OK, {                                                            \
            KEFIR_FREE(mem, specifier);                                                            \
            return NULL;                                                                           \
        });                                                                                        \
        return specifier;                                                                          \
    }

TYPE_SPECIFIER(void, KEFIR_AST_TYPE_SPECIFIER_VOID)
TYPE_SPECIFIER(char, KEFIR_AST_TYPE_SPECIFIER_CHAR)
TYPE_SPECIFIER(short, KEFIR_AST_TYPE_SPECIFIER_SHORT)
TYPE_SPECIFIER(int, KEFIR_AST_TYPE_SPECIFIER_INT)
TYPE_SPECIFIER(long, KEFIR_AST_TYPE_SPECIFIER_LONG)
TYPE_SPECIFIER(float, KEFIR_AST_TYPE_SPECIFIER_FLOAT)
TYPE_SPECIFIER(double, KEFIR_AST_TYPE_SPECIFIER_DOUBLE)
TYPE_SPECIFIER(signed, KEFIR_AST_TYPE_SPECIFIER_SIGNED)
TYPE_SPECIFIER(unsigned, KEFIR_AST_TYPE_SPECIFIER_UNSIGNED)
TYPE_SPECIFIER(bool, KEFIR_AST_TYPE_SPECIFIER_BOOL)
TYPE_SPECIFIER(complex, KEFIR_AST_TYPE_SPECIFIER_COMPLEX)
TYPE_SPECIFIER(va_list, KEFIR_AST_TYPE_SPECIFIER_VA_LIST)

#undef TYPE_SPECIFIER

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_atomic(struct kefir_mem *mem,
                                                                       struct kefir_ast_node_base *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_ATOMIC;
    specifier->type_specifier.value.atomic_type = type;
    kefir_result_t res = kefir_source_location_empty(&specifier->source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, specifier);
        return NULL;
    });
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_struct(
    struct kefir_mem *mem, struct kefir_ast_structure_specifier *structure) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(structure != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_STRUCT;
    specifier->type_specifier.value.structure = structure;
    kefir_result_t res = kefir_source_location_empty(&specifier->source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, specifier);
        return NULL;
    });
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_union(struct kefir_mem *mem,
                                                                      struct kefir_ast_structure_specifier *structure) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(structure != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_UNION;
    specifier->type_specifier.value.structure = structure;
    kefir_result_t res = kefir_source_location_empty(&specifier->source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, specifier);
        return NULL;
    });
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_enum(struct kefir_mem *mem,
                                                                     struct kefir_ast_enum_specifier *enumeration) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(enumeration != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_ENUM;
    specifier->type_specifier.value.enumeration = enumeration;
    kefir_result_t res = kefir_source_location_empty(&specifier->source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, specifier);
        return NULL;
    });
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_typedef(struct kefir_mem *mem,
                                                                        struct kefir_symbol_table *symbols,
                                                                        const char *literal) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(literal != NULL, NULL);

    if (symbols != NULL) {
        literal = kefir_symbol_table_insert(mem, symbols, literal, NULL);
        REQUIRE(literal != NULL, NULL);
    }

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_TYPEDEF;
    specifier->type_specifier.value.type_name = literal;
    kefir_result_t res = kefir_source_location_empty(&specifier->source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, specifier);
        return NULL;
    });
    return specifier;
}

#define STORAGE_CLASS_SPECIFIER(_id, _spec)                                                                 \
    struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_##_id(struct kefir_mem *mem) { \
        REQUIRE(mem != NULL, NULL);                                                                         \
        struct kefir_ast_declarator_specifier *specifier =                                                  \
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));                               \
        REQUIRE(specifier != NULL, NULL);                                                                   \
                                                                                                            \
        specifier->klass = KEFIR_AST_STORAGE_CLASS_SPECIFIER;                                               \
        specifier->storage_class = (_spec);                                                                 \
        kefir_result_t res = kefir_source_location_empty(&specifier->source_location);                      \
        REQUIRE_ELSE(res == KEFIR_OK, {                                                                     \
            KEFIR_FREE(mem, specifier);                                                                     \
            return NULL;                                                                                    \
        });                                                                                                 \
        return specifier;                                                                                   \
    }

STORAGE_CLASS_SPECIFIER(typedef, KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF)
STORAGE_CLASS_SPECIFIER(extern, KEFIR_AST_STORAGE_SPECIFIER_EXTERN)
STORAGE_CLASS_SPECIFIER(static, KEFIR_AST_STORAGE_SPECIFIER_STATIC)
STORAGE_CLASS_SPECIFIER(thread_local, KEFIR_AST_STORAGE_SPECIFIER_THREAD_LOCAL)
STORAGE_CLASS_SPECIFIER(auto, KEFIR_AST_STORAGE_SPECIFIER_AUTO)
STORAGE_CLASS_SPECIFIER(register, KEFIR_AST_STORAGE_SPECIFIER_REGISTER)

#undef STORAGE_CLASS_SPECIFIER

#define TYPE_QUALIFIER(_id, _spec)                                                                 \
    struct kefir_ast_declarator_specifier *kefir_ast_type_qualifier_##_id(struct kefir_mem *mem) { \
        REQUIRE(mem != NULL, NULL);                                                                \
        struct kefir_ast_declarator_specifier *specifier =                                         \
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));                      \
        REQUIRE(specifier != NULL, NULL);                                                          \
                                                                                                   \
        specifier->klass = KEFIR_AST_TYPE_QUALIFIER;                                               \
        specifier->type_qualifier = (_spec);                                                       \
        kefir_result_t res = kefir_source_location_empty(&specifier->source_location);             \
        REQUIRE_ELSE(res == KEFIR_OK, {                                                            \
            KEFIR_FREE(mem, specifier);                                                            \
            return NULL;                                                                           \
        });                                                                                        \
        return specifier;                                                                          \
    }

TYPE_QUALIFIER(const, KEFIR_AST_TYPE_QUALIFIER_CONST)
TYPE_QUALIFIER(restrict, KEFIR_AST_TYPE_QUALIFIER_RESTRICT)
TYPE_QUALIFIER(volatile, KEFIR_AST_TYPE_QUALIFIER_VOLATILE)
TYPE_QUALIFIER(atomic, KEFIR_AST_TYPE_QUALIFIER_ATOMIC)

#undef TYPE_QUALIFIER

#define FUNCTION_SPECIFIER(_id, _spec)                                                                 \
    struct kefir_ast_declarator_specifier *kefir_ast_function_specifier_##_id(struct kefir_mem *mem) { \
        REQUIRE(mem != NULL, NULL);                                                                    \
        struct kefir_ast_declarator_specifier *specifier =                                             \
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));                          \
        REQUIRE(specifier != NULL, NULL);                                                              \
                                                                                                       \
        specifier->klass = KEFIR_AST_FUNCTION_SPECIFIER;                                               \
        specifier->function_specifier = (_spec);                                                       \
        kefir_result_t res = kefir_source_location_empty(&specifier->source_location);                 \
        REQUIRE_ELSE(res == KEFIR_OK, {                                                                \
            KEFIR_FREE(mem, specifier);                                                                \
            return NULL;                                                                               \
        });                                                                                            \
        return specifier;                                                                              \
    }

FUNCTION_SPECIFIER(inline, KEFIR_AST_FUNCTION_SPECIFIER_TYPE_INLINE)
FUNCTION_SPECIFIER(noreturn, KEFIR_AST_FUNCTION_SPECIFIER_TYPE_NORETURN)

#undef FUNCTION_SPECIFIER

struct kefir_ast_declarator_specifier *kefir_ast_alignment_specifier(struct kefir_mem *mem,
                                                                     struct kefir_ast_node_base *alignment) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(alignment != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->klass = KEFIR_AST_ALIGNMENT_SPECIFIER;
    specifier->alignment_specifier = alignment;
    kefir_result_t res = kefir_source_location_empty(&specifier->source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, specifier);
        return NULL;
    });
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_declarator_specifier_clone(
    struct kefir_mem *mem, const struct kefir_ast_declarator_specifier *specifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(specifier != NULL, NULL);

    struct kefir_ast_declarator_specifier *clone = NULL;
    switch (specifier->klass) {
        case KEFIR_AST_TYPE_SPECIFIER: {
            switch (specifier->type_specifier.specifier) {
                case KEFIR_AST_TYPE_SPECIFIER_VOID:
                    clone = kefir_ast_type_specifier_void(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_CHAR:
                    clone = kefir_ast_type_specifier_char(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_SHORT:
                    clone = kefir_ast_type_specifier_short(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_INT:
                    clone = kefir_ast_type_specifier_int(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_LONG:
                    clone = kefir_ast_type_specifier_long(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_FLOAT:
                    clone = kefir_ast_type_specifier_float(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_DOUBLE:
                    clone = kefir_ast_type_specifier_double(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_SIGNED:
                    clone = kefir_ast_type_specifier_signed(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_UNSIGNED:
                    clone = kefir_ast_type_specifier_unsigned(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_BOOL:
                    clone = kefir_ast_type_specifier_bool(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_COMPLEX:
                    clone = kefir_ast_type_specifier_complex(mem);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ATOMIC:
                    clone = kefir_ast_type_specifier_atomic(
                        mem, KEFIR_AST_NODE_CLONE(mem, specifier->type_specifier.value.atomic_type));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_STRUCT:
                    clone = kefir_ast_type_specifier_struct(
                        mem, kefir_ast_structure_specifier_clone(mem, specifier->type_specifier.value.structure));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_UNION:
                    clone = kefir_ast_type_specifier_union(
                        mem, kefir_ast_structure_specifier_clone(mem, specifier->type_specifier.value.structure));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ENUM:
                    clone = kefir_ast_type_specifier_enum(
                        mem, kefir_ast_enum_specifier_clone(mem, specifier->type_specifier.value.enumeration));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_TYPEDEF:
                    clone = kefir_ast_type_specifier_typedef(mem, NULL, specifier->type_specifier.value.type_name);
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_VA_LIST:
                    clone = kefir_ast_type_specifier_va_list(mem);
                    break;

                default:
                    // Intentionally left blank
                    break;
            }
        } break;

        case KEFIR_AST_TYPE_QUALIFIER: {
            switch (specifier->type_qualifier) {
                case KEFIR_AST_TYPE_QUALIFIER_CONST:
                    clone = kefir_ast_type_qualifier_const(mem);
                    break;

                case KEFIR_AST_TYPE_QUALIFIER_RESTRICT:
                    clone = kefir_ast_type_qualifier_restrict(mem);
                    break;

                case KEFIR_AST_TYPE_QUALIFIER_VOLATILE:
                    clone = kefir_ast_type_qualifier_volatile(mem);
                    break;

                case KEFIR_AST_TYPE_QUALIFIER_ATOMIC:
                    clone = kefir_ast_type_qualifier_atomic(mem);
                    break;

                default:
                    // Intentionally left blank
                    break;
            }
        } break;

        case KEFIR_AST_STORAGE_CLASS_SPECIFIER: {
            switch (specifier->storage_class) {
                case KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF:
                    clone = kefir_ast_storage_class_specifier_typedef(mem);
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_EXTERN:
                    clone = kefir_ast_storage_class_specifier_extern(mem);
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_STATIC:
                    clone = kefir_ast_storage_class_specifier_static(mem);
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_THREAD_LOCAL:
                    clone = kefir_ast_storage_class_specifier_thread_local(mem);
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_AUTO:
                    clone = kefir_ast_storage_class_specifier_auto(mem);
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_REGISTER:
                    clone = kefir_ast_storage_class_specifier_register(mem);
                    break;

                default:
                    // Intentionally left blank
                    break;
            }
        } break;

        case KEFIR_AST_FUNCTION_SPECIFIER: {
            switch (specifier->function_specifier) {
                case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_NORETURN:
                    clone = kefir_ast_function_specifier_noreturn(mem);
                    break;

                case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_INLINE:
                    clone = kefir_ast_function_specifier_inline(mem);
                    break;

                default:
                    // Intentionally left blank
                    break;
            }
        } break;

        case KEFIR_AST_ALIGNMENT_SPECIFIER:
            clone = kefir_ast_alignment_specifier(mem, KEFIR_AST_NODE_CLONE(mem, specifier->alignment_specifier));
            break;
    }

    if (clone != NULL) {
        clone->source_location = specifier->source_location;
    }
    return clone;
}

kefir_result_t kefir_ast_declarator_specifier_free(struct kefir_mem *mem,
                                                   struct kefir_ast_declarator_specifier *specifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier"));

    switch (specifier->klass) {
        case KEFIR_AST_TYPE_SPECIFIER:
            switch (specifier->type_specifier.specifier) {
                case KEFIR_AST_TYPE_SPECIFIER_ATOMIC:
                    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, specifier->type_specifier.value.atomic_type));
                    specifier->type_specifier.value.atomic_type = NULL;
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_STRUCT:
                case KEFIR_AST_TYPE_SPECIFIER_UNION:
                    REQUIRE_OK(kefir_ast_structure_specifier_free(mem, specifier->type_specifier.value.structure));
                    specifier->type_specifier.value.structure = NULL;
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ENUM:
                    REQUIRE_OK(kefir_ast_enum_specifier_free(mem, specifier->type_specifier.value.enumeration));
                    specifier->type_specifier.value.enumeration = NULL;
                    break;

                default:
                    // Nothing to do
                    break;
            }
            break;

        case KEFIR_AST_ALIGNMENT_SPECIFIER:
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, specifier->alignment_specifier));
            specifier->alignment_specifier = NULL;
            break;

        case KEFIR_AST_TYPE_QUALIFIER:
        case KEFIR_AST_STORAGE_CLASS_SPECIFIER:
        case KEFIR_AST_FUNCTION_SPECIFIER:
            // Nothing to do
            break;
    }

    KEFIR_FREE(mem, specifier);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_qualifier_list_init(struct kefir_ast_type_qualifier_list *list) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type qualifier list"));

    REQUIRE_OK(kefir_list_init(&list->list));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_qualifier_list_free(struct kefir_mem *mem, struct kefir_ast_type_qualifier_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type qualifier list"));

    REQUIRE_OK(kefir_list_free(mem, &list->list));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_qualifier_list_append(struct kefir_mem *mem, struct kefir_ast_type_qualifier_list *list,
                                                    kefir_ast_type_qualifier_type_t qualifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type qualifier list"));

    REQUIRE_OK(
        kefir_list_insert_after(mem, &list->list, kefir_list_tail(&list->list), (void *) ((kefir_uptr_t) qualifier)));
    return KEFIR_OK;
}

struct kefir_list_entry *kefir_ast_type_qualifier_list_iter(const struct kefir_ast_type_qualifier_list *list,
                                                            kefir_ast_type_qualifier_type_t *value) {
    REQUIRE(list != NULL, NULL);

    struct kefir_list_entry *iter = kefir_list_head(&list->list);
    if (iter != NULL) {
        ASSIGN_PTR(value, (kefir_ast_type_qualifier_type_t) ((kefir_uptr_t) iter->value));
    }
    return iter;
}

kefir_result_t kefir_ast_type_qualifier_list_next(const struct kefir_list_entry **iter,
                                                  kefir_ast_type_qualifier_type_t *value) {
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to list entry iterator"));
    REQUIRE(*iter != NULL, KEFIR_OK);

    *iter = (*iter)->next;
    if (*iter != NULL) {
        ASSIGN_PTR(value, (kefir_ast_type_qualifier_type_t) ((kefir_uptr_t) (*iter)->value));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_qualifier_list_remove(struct kefir_mem *mem, struct kefir_ast_type_qualifier_list *list,
                                                    struct kefir_list_entry *iter) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type qualifier list"));
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list iterator"));

    REQUIRE_OK(kefir_list_pop(mem, &list->list, iter));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_qualifier_list_clone(struct kefir_mem *mem, struct kefir_ast_type_qualifier_list *dst,
                                                   const struct kefir_ast_type_qualifier_list *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(dst != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid destination AST type qualifier list"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source AST type qualifier list"));

    kefir_ast_type_qualifier_type_t value;
    for (const struct kefir_list_entry *iter = kefir_ast_type_qualifier_list_iter(src, &value); iter != NULL;
         kefir_ast_type_qualifier_list_next(&iter, &value)) {
        REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, dst, value));
    }
    return KEFIR_OK;
}
