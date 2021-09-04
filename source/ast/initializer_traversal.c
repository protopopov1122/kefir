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

#include "kefir/ast/initializer_traversal.h"
#include "kefir/ast/analyzer/type_traversal.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define INVOKE_TRAVERSAL(_traversal, _callback, ...)                                 \
    do {                                                                             \
        if ((_traversal)->_callback != NULL) {                                       \
            REQUIRE_OK((_traversal)->_callback(__VA_ARGS__, (_traversal)->payload)); \
        }                                                                            \
    } while (0)

#define INVOKE_TRAVERSAL_CHAIN(_res, _traversal, _callback, ...)                                \
    do {                                                                                        \
        if ((_traversal)->_callback != NULL) {                                                  \
            REQUIRE_CHAIN((_res), (_traversal)->_callback(__VA_ARGS__, (_traversal)->payload)); \
        }                                                                                       \
    } while (0)

static kefir_result_t kefi_ast_traverse_initializer_impl(struct kefir_mem *, const struct kefir_ast_context *,
                                                         const struct kefir_ast_designator *,
                                                         const struct kefir_ast_initializer *,
                                                         const struct kefir_ast_type *,
                                                         const struct kefir_ast_initializer_traversal *);

static kefir_result_t traverse_scalar(const struct kefir_ast_designator *designator,
                                      const struct kefir_ast_initializer *initializer,
                                      const struct kefir_ast_initializer_traversal *traversal) {
    struct kefir_ast_node_base *expr = kefir_ast_initializer_head(initializer);
    REQUIRE(expr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Scalar initializer list cannot be empty"));
    INVOKE_TRAVERSAL(traversal, visit_value, designator, expr);
    return KEFIR_OK;
}

static kefir_bool_t is_char_array(const struct kefir_ast_type *type, void *payload) {
    UNUSED(payload);
    return type->tag == KEFIR_AST_TYPE_ARRAY &&
           KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_unqualified_type(type->array_type.element_type));
}

static kefir_bool_t is_array_of(const struct kefir_ast_type *type, void *payload) {
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(const struct kefir_ast_type *, expected_type, payload);
    return type->tag == KEFIR_AST_TYPE_ARRAY &&
           KEFIR_AST_TYPE_SAME(kefir_ast_unqualified_type(type->array_type.element_type), expected_type);
}

static kefir_result_t string_literal_stop_fn(struct kefir_ast_node_base *node,
                                             const struct kefir_ast_type_traits *type_traits,
                                             kefir_bool_t (**stop_fn)(const struct kefir_ast_type *, void *),
                                             void **stop_payload) {
    switch (node->properties.expression_props.string_literal.type) {
        case KEFIR_AST_STRING_LITERAL_MULTIBYTE:
        case KEFIR_AST_STRING_LITERAL_UNICODE8:
            *stop_fn = is_char_array;
            *stop_payload = NULL;
            break;

        case KEFIR_AST_STRING_LITERAL_UNICODE16:
            *stop_fn = is_array_of;
            *stop_payload = (void *) type_traits->unicode16_char_type;
            break;

        case KEFIR_AST_STRING_LITERAL_UNICODE32:
            *stop_fn = is_array_of;
            *stop_payload = (void *) type_traits->unicode32_char_type;
            break;

        case KEFIR_AST_STRING_LITERAL_WIDE:
            *stop_fn = is_array_of;
            *stop_payload = (void *) type_traits->wide_char_type;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected string literal type");
    }
    return KEFIR_OK;
}

static kefir_result_t layer_designator(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                       struct kefir_ast_designator *current_designator,
                                       const struct kefir_ast_type_traversal_layer *layer,
                                       struct kefir_ast_designator **result) {
    struct kefir_ast_designator *designator = current_designator;

    if (designator == NULL) {
        designator = kefir_ast_type_traversal_layer_designator(mem, symbols, layer);
        REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to derive a designator"));
    }

    *result = designator;
    return KEFIR_OK;
}

static kefir_result_t assign_string(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                    struct kefir_ast_initializer_list_entry *entry,
                                    struct kefir_ast_type_traversal *traversal,
                                    const struct kefir_ast_initializer_traversal *initializer_traversal) {
    const struct kefir_ast_type *type = NULL;
    const struct kefir_ast_type_traversal_layer *layer = NULL;
    struct kefir_ast_designator *designator_layer = NULL;
    kefir_bool_t (*stop_fn)(const struct kefir_ast_type *, void *) = NULL;
    void *stop_payload = NULL;
    struct kefir_ast_node_base *node = entry->value->expression;
    REQUIRE_OK(string_literal_stop_fn(node, context->type_traits, &stop_fn, &stop_payload));

    REQUIRE_OK(kefir_ast_type_traversal_next_recursive2(mem, traversal, stop_fn, stop_payload, &type, &layer));
    REQUIRE_OK(layer_designator(mem, context->symbols, entry->designator, layer, &designator_layer));

    kefir_result_t res = KEFIR_OK;
    if (stop_fn(type, stop_payload) && (type->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED ||
                                        type->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC)) {
        kefir_size_t length = MIN((kefir_size_t) type->array_type.const_length->value.integer,
                                  entry->value->expression->properties.expression_props.string_literal.length);
        INVOKE_TRAVERSAL_CHAIN(&res, initializer_traversal, visit_string_literal, designator_layer, node,
                               node->properties.expression_props.string_literal.type,
                               node->properties.expression_props.string_literal.content, length);
    } else {
        INVOKE_TRAVERSAL_CHAIN(&res, initializer_traversal, visit_value, designator_layer, entry->value->expression);
    }

    REQUIRE_ELSE(res == KEFIR_OK, {
        if (entry->designator == NULL && designator_layer != NULL) {
            kefir_ast_designator_free(mem, designator_layer);
        }
        return res;
    });

    if (entry->designator == NULL && designator_layer != NULL) {
        REQUIRE_OK(kefir_ast_designator_free(mem, designator_layer));
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_aggregate_union(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                               const struct kefir_ast_initializer *initializer,
                                               struct kefir_ast_type_traversal *traversal,
                                               const struct kefir_ast_initializer_traversal *initializer_traversal) {
    const struct kefir_list_entry *init_iter = kefir_list_head(&initializer->list.initializers);
    for (; init_iter != NULL; kefir_list_next(&init_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, entry, init_iter->value);
        if (entry->designator != NULL) {
            REQUIRE_OK(kefir_ast_type_traversal_navigate(mem, traversal, entry->designator));
        } else if (kefir_ast_type_traversal_empty(traversal)) {
            continue;
        }

        const struct kefir_ast_type_traversal_layer *layer = NULL;
        struct kefir_ast_designator *designator_layer = NULL;
        kefir_result_t res = KEFIR_OK;

        if (entry->value->type == KEFIR_AST_INITIALIZER_LIST) {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, &layer));
            REQUIRE_OK(layer_designator(mem, context->symbols, entry->designator, layer, &designator_layer));

            INVOKE_TRAVERSAL_CHAIN(&res, initializer_traversal, visit_initializer_list, designator_layer, entry->value);
        } else if (entry->value->expression->properties.expression_props.string_literal.content != NULL) {
            REQUIRE_OK(assign_string(mem, context, entry, traversal, initializer_traversal));
        } else if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(entry->value->expression->properties.type)) {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next_recursive(mem, traversal, &type, &layer));
            REQUIRE_OK(layer_designator(mem, context->symbols, entry->designator, layer, &designator_layer));

            INVOKE_TRAVERSAL_CHAIN(&res, initializer_traversal, visit_value, designator_layer,
                                   entry->value->expression);
        } else {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, &layer));
            res = kefir_ast_node_assignable(mem, context, entry->value->expression, kefir_ast_unqualified_type(type));
            while (res == KEFIR_NO_MATCH) {
                REQUIRE_OK(kefir_ast_type_traversal_step(mem, traversal));
                REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, &layer));
                res =
                    kefir_ast_node_assignable(mem, context, entry->value->expression, kefir_ast_unqualified_type(type));
            }
            REQUIRE_OK(res);
            REQUIRE_OK(layer_designator(mem, context->symbols, entry->designator, layer, &designator_layer));

            INVOKE_TRAVERSAL_CHAIN(&res, initializer_traversal, visit_value, designator_layer,
                                   entry->value->expression);
        }

        REQUIRE_ELSE(res == KEFIR_OK, {
            if (entry->designator == NULL && designator_layer != NULL) {
                kefir_ast_designator_free(mem, designator_layer);
            }
            return res;
        });

        if (entry->designator == NULL && designator_layer != NULL) {
            REQUIRE_OK(kefir_ast_designator_free(mem, designator_layer));
        }
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_struct_union(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                            const struct kefir_ast_type *type,
                                            const struct kefir_ast_designator *designator,
                                            const struct kefir_ast_initializer *initializer,
                                            const struct kefir_ast_initializer_traversal *initializer_traversal) {
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot initialize incomplete object type"));
    if (initializer->type == KEFIR_AST_INITIALIZER_EXPRESSION) {
        INVOKE_TRAVERSAL(initializer_traversal, visit_value, designator, initializer->expression);
    } else {
        INVOKE_TRAVERSAL(initializer_traversal, begin_struct_union, designator, initializer);
        struct kefir_ast_type_traversal traversal;
        REQUIRE_OK(kefir_ast_type_traversal_init(mem, &traversal, type));
        kefir_result_t res = traverse_aggregate_union(mem, context, initializer, &traversal, initializer_traversal);
        REQUIRE_OK(kefir_ast_type_traversal_free(mem, &traversal));
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_array(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                     const struct kefir_ast_designator *designator, const struct kefir_ast_type *type,
                                     const struct kefir_ast_initializer *initializer,
                                     const struct kefir_ast_initializer_traversal *initializer_traversal) {
    struct kefir_ast_node_base *head_expr = kefir_ast_initializer_head(initializer);
    kefir_bool_t is_string = false;

    if (head_expr != NULL && head_expr->properties.expression_props.string_literal.content != NULL) {
        kefir_bool_t (*stop_fn)(const struct kefir_ast_type *, void *) = NULL;
        void *stop_payload = NULL;
        REQUIRE_OK(string_literal_stop_fn(head_expr, context->type_traits, &stop_fn, &stop_payload));
        if (stop_fn(type, stop_payload)) {
            INVOKE_TRAVERSAL(initializer_traversal, visit_string_literal, designator, head_expr,
                             head_expr->properties.expression_props.string_literal.type,
                             head_expr->properties.expression_props.string_literal.content,
                             head_expr->properties.expression_props.string_literal.length);
            is_string = true;
        }
    }

    if (!is_string) {
        REQUIRE(initializer->type == KEFIR_AST_INITIALIZER_LIST,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to initialize array by non-string literal expression"));

        INVOKE_TRAVERSAL(initializer_traversal, begin_array, designator, initializer);
        struct kefir_ast_type_traversal traversal;
        REQUIRE_OK(kefir_ast_type_traversal_init(mem, &traversal, type));
        kefir_result_t res = traverse_aggregate_union(mem, context, initializer, &traversal, initializer_traversal);
        REQUIRE_OK(kefir_ast_type_traversal_free(mem, &traversal));
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

static kefir_result_t kefi_ast_traverse_initializer_impl(
    struct kefir_mem *mem, const struct kefir_ast_context *context, const struct kefir_ast_designator *designator,
    const struct kefir_ast_initializer *initializer, const struct kefir_ast_type *type,
    const struct kefir_ast_initializer_traversal *initializer_traversal) {
    type = kefir_ast_unqualified_type(type);

    if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(type)) {
        REQUIRE_OK(traverse_scalar(designator, initializer, initializer_traversal));
    } else if (type->tag == KEFIR_AST_TYPE_ARRAY) {
        REQUIRE_OK(traverse_array(mem, context, designator, type, initializer, initializer_traversal));
    } else if (type->tag == KEFIR_AST_TYPE_STRUCTURE || type->tag == KEFIR_AST_TYPE_UNION) {
        REQUIRE_OK(traverse_struct_union(mem, context, type, designator, initializer, initializer_traversal));
    } else {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot initialize incomplete object type");
    }
    return KEFIR_OK;
}

kefir_result_t kefi_ast_traverse_initializer(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                             const struct kefir_ast_initializer *initializer,
                                             const struct kefir_ast_type *type,
                                             const struct kefir_ast_initializer_traversal *initializer_traversal) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST initializer"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(initializer_traversal != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer traversal"));

    return kefi_ast_traverse_initializer_impl(mem, context, NULL, initializer, type, initializer_traversal);
}
