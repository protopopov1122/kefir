/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

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

#include "kefir/ast/initializer.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct kefir_ast_initializer_designation *kefir_ast_new_initializer_member_designation(
    struct kefir_mem *mem, struct kefir_symbol_table *symbols, const char *identifier,
    struct kefir_ast_initializer_designation *next) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);

    if (symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_ast_initializer_designation *designation =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer_designation));
    REQUIRE(designation != NULL, NULL);

    designation->indexed = false;
    designation->identifier = identifier;
    designation->next = next;
    return designation;
}

struct kefir_ast_initializer_designation *kefir_ast_new_initializer_index_designation(
    struct kefir_mem *mem, struct kefir_ast_node_base *index, struct kefir_ast_initializer_designation *next) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(index != NULL, NULL);

    struct kefir_ast_initializer_designation *designation =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer_designation));
    REQUIRE(designation != NULL, NULL);

    designation->indexed = true;
    designation->index = index;
    designation->next = next;
    return designation;
}
struct kefir_ast_initializer_designation *kefir_ast_initializer_designation_clone(
    struct kefir_mem *mem, struct kefir_ast_initializer_designation *designation) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(designation != NULL, NULL);

    struct kefir_ast_initializer_designation *next_clone = NULL;
    if (designation->next != NULL) {
        next_clone = kefir_ast_initializer_designation_clone(mem, designation->next);
        REQUIRE(next_clone != NULL, NULL);
    }

    struct kefir_ast_initializer_designation *clone =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer_designation));
    REQUIRE_ELSE(clone != NULL, {
        kefir_ast_initializer_designation_free(mem, next_clone);
        return NULL;
    });
    clone->indexed = designation->indexed;
    clone->next = next_clone;

    if (designation->indexed) {
        clone->index = KEFIR_AST_NODE_CLONE(mem, designation->index);
        REQUIRE_ELSE(clone->index != NULL, {
            kefir_ast_initializer_designation_free(mem, clone->next);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    } else {
        clone->identifier = designation->identifier;
    }
    return clone;
}

kefir_result_t kefir_ast_initializer_designation_free(struct kefir_mem *mem,
                                                      struct kefir_ast_initializer_designation *designation) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(designation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer designation"));

    if (designation->next != NULL) {
        REQUIRE_OK(kefir_ast_initializer_designation_free(mem, designation->next));
        designation->next = NULL;
    }

    if (designation->indexed) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, designation->index));
        designation->index = NULL;
    } else {
        designation->identifier = NULL;
    }
    KEFIR_FREE(mem, designation);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_evaluate_initializer_designation(struct kefir_mem *mem,
                                                          const struct kefir_ast_context *context,
                                                          const struct kefir_ast_initializer_designation *designation,
                                                          struct kefir_ast_designator **designator_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(designation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer designation"));
    REQUIRE(designator_ptr != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST designator"));

    struct kefir_ast_designator *next_designator = NULL;
    if (designation->next != NULL) {
        REQUIRE_OK(kefir_ast_evaluate_initializer_designation(mem, context, designation->next, &next_designator));
    }

    struct kefir_ast_designator *designator = NULL;
    if (designation->indexed) {
        kefir_result_t res = kefir_ast_analyze_node(mem, context, designation->index);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_designator_free(mem, next_designator);
            return res;
        });

        struct kefir_ast_constant_expression_value value;
        res = kefir_ast_constant_expression_value_evaluate(mem, context, designation->index, &value);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_designator_free(mem, next_designator);
            return res;
        });
        REQUIRE_ELSE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER, {
            kefir_ast_designator_free(mem, next_designator);
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "AST designator index must be an integral constant expression");
        });

        designator = kefir_ast_new_index_designator(mem, value.integer, next_designator);
        REQUIRE_ELSE(designator != NULL, {
            kefir_ast_designator_free(mem, next_designator);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allicate AST index designator");
        });
    } else {
        designator = kefir_ast_new_member_designator(mem, context->symbols, designation->identifier, next_designator);
        REQUIRE_ELSE(designator != NULL, {
            kefir_ast_designator_free(mem, next_designator);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allicate AST member designator");
        });
    }

    *designator_ptr = designator;
    return KEFIR_OK;
}

static kefir_result_t list_entry_removal(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                         void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, list_entry, entry->value);
    if (list_entry->designator != NULL) {
        REQUIRE_OK(kefir_ast_designator_free(mem, list_entry->designator));
        list_entry->designator = NULL;
    }
    if (list_entry->designation != NULL) {
        REQUIRE_OK(kefir_ast_initializer_designation_free(mem, list_entry->designation));
        list_entry->designation = NULL;
    }
    REQUIRE_OK(kefir_ast_initializer_free(mem, list_entry->value));
    KEFIR_FREE(mem, list_entry);
    return KEFIR_OK;
}

struct kefir_ast_initializer *kefir_ast_new_expression_initializer(struct kefir_mem *mem,
                                                                   struct kefir_ast_node_base *expr) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(expr != NULL, NULL);

    struct kefir_ast_initializer *initializer = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer));
    REQUIRE(initializer != NULL, NULL);
    initializer->type = KEFIR_AST_INITIALIZER_EXPRESSION;
    initializer->expression = expr;
    return initializer;
}

struct kefir_ast_initializer *kefir_ast_new_list_initializer(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_initializer *initializer = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer));
    REQUIRE(initializer != NULL, NULL);
    initializer->type = KEFIR_AST_INITIALIZER_LIST;
    kefir_result_t res = kefir_ast_initializer_list_init(&initializer->list);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, initializer);
        return NULL;
    });
    return initializer;
}

kefir_result_t kefir_ast_initializer_free(struct kefir_mem *mem, struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer"));

    switch (initializer->type) {
        case KEFIR_AST_INITIALIZER_EXPRESSION:
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, initializer->expression));
            break;

        case KEFIR_AST_INITIALIZER_LIST:
            REQUIRE_OK(kefir_ast_initializer_list_free(mem, &initializer->list));
            break;
    }
    KEFIR_FREE(mem, initializer);
    return KEFIR_OK;
}

struct kefir_ast_node_base *kefir_ast_initializer_head(const struct kefir_ast_initializer *initializer) {
    REQUIRE(initializer != NULL, NULL);
    if (initializer->type == KEFIR_AST_INITIALIZER_EXPRESSION) {
        return initializer->expression;
    } else {
        const struct kefir_list_entry *head_entry = kefir_list_head(&initializer->list.initializers);
        REQUIRE(head_entry != NULL, NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, entry, head_entry->value);
        return kefir_ast_initializer_head(entry->value);
    }
}

struct kefir_ast_initializer *kefir_ast_initializer_clone(struct kefir_mem *mem,
                                                          const struct kefir_ast_initializer *src) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(src != NULL, NULL);

    struct kefir_ast_initializer *dst = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer));
    REQUIRE(dst != NULL, NULL);
    dst->type = src->type;
    switch (src->type) {
        case KEFIR_AST_INITIALIZER_EXPRESSION:
            dst->expression = KEFIR_AST_NODE_CLONE(mem, src->expression);
            REQUIRE_ELSE(dst->expression != NULL, {
                KEFIR_FREE(mem, dst);
                return NULL;
            });
            break;

        case KEFIR_AST_INITIALIZER_LIST: {
            kefir_result_t res = kefir_ast_initializer_list_clone(mem, &dst->list, &src->list);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_FREE(mem, dst);
                return NULL;
            });
        } break;
    }
    return dst;
}

kefir_result_t kefir_ast_initializer_list_init(struct kefir_ast_initializer_list *list) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer list"));
    REQUIRE_OK(kefir_list_init(&list->initializers));
    REQUIRE_OK(kefir_list_on_remove(&list->initializers, list_entry_removal, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_initializer_list_free(struct kefir_mem *mem, struct kefir_ast_initializer_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer list"));
    REQUIRE_OK(kefir_list_free(mem, &list->initializers));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_initializer_list_append(struct kefir_mem *mem, struct kefir_ast_initializer_list *list,
                                                 struct kefir_ast_initializer_designation *designation,
                                                 struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer list"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer"));

    struct kefir_ast_initializer_list_entry *entry = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer_list_entry));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST initializer list entry"));
    entry->designator = NULL;
    entry->designation = designation;
    entry->value = initializer;
    kefir_result_t res = kefir_list_insert_after(mem, &list->initializers, kefir_list_tail(&list->initializers), entry);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, entry);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_initializer_list_clone(struct kefir_mem *mem, struct kefir_ast_initializer_list *dst,
                                                const struct kefir_ast_initializer_list *src) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(src != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid source AST initializer list"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid destination AST initializer list"));

    REQUIRE_OK(kefir_list_init(&dst->initializers));
    REQUIRE_OK(kefir_list_on_remove(&dst->initializers, list_entry_removal, NULL));
    for (const struct kefir_list_entry *iter = kefir_list_head(&src->initializers); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, entry, iter->value);
        struct kefir_ast_initializer_list_entry *clone =
            KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer_list_entry));
        REQUIRE_ELSE(clone != NULL, {
            kefir_list_free(mem, &dst->initializers);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST initializer list entry");
        });
        clone->designator = kefir_ast_designator_clone(mem, entry->designator);
        if (entry->designator != NULL) {
            REQUIRE_ELSE(clone->designator != NULL, {
                KEFIR_FREE(mem, clone);
                kefir_list_free(mem, &dst->initializers);
                return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to clone AST designator");
            });
        }
        clone->designation = kefir_ast_initializer_designation_clone(mem, entry->designation);
        if (entry->designation != NULL) {
            REQUIRE_ELSE(clone->designation != NULL, {
                if (clone->designator != NULL) {
                    kefir_ast_designator_free(mem, clone->designator);
                }
                KEFIR_FREE(mem, clone);
                kefir_list_free(mem, &dst->initializers);
                return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to clone AST designator");
            });
        }
        clone->value = kefir_ast_initializer_clone(mem, entry->value);
        REQUIRE_ELSE(clone->value != NULL, {
            if (clone->designation != NULL) {
                kefir_ast_initializer_designation_free(mem, clone->designation);
            }
            if (clone->designator != NULL) {
                kefir_ast_designator_free(mem, clone->designator);
            }
            KEFIR_FREE(mem, clone);
            kefir_list_free(mem, &dst->initializers);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to clone AST initializer");
        });
        kefir_result_t res =
            kefir_list_insert_after(mem, &dst->initializers, kefir_list_tail(&dst->initializers), clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_initializer_free(mem, clone->value);
            if (clone->designation != NULL) {
                kefir_ast_initializer_designation_free(mem, clone->designation);
            }
            if (clone->designator != NULL) {
                kefir_ast_designator_free(mem, clone->designator);
            }
            KEFIR_FREE(mem, clone);
            kefir_list_free(mem, &dst->initializers);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to clone AST initializer");
        });
    }
    return KEFIR_OK;
}
