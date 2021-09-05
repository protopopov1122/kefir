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

#include "kefir/ast/analyzer/initializer.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/type_traversal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include <stdio.h>

static kefir_result_t preanalyze_initializer(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                             const struct kefir_ast_initializer *initializer,
                                             struct kefir_ast_initializer_properties *properties) {
    if (initializer->type == KEFIR_AST_INITIALIZER_EXPRESSION) {
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, initializer->expression));
        if (properties != NULL && !initializer->expression->properties.expression_props.constant_expression) {
            properties->constant = false;
        }
    } else {
        for (const struct kefir_list_entry *iter = kefir_list_head(&initializer->list.initializers); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, entry, iter->value);
            if (entry->designation != NULL && entry->designator == NULL) {
                REQUIRE_OK(
                    kefir_ast_evaluate_initializer_designation(mem, context, entry->designation, &entry->designator));
            }
            REQUIRE_OK(preanalyze_initializer(mem, context, entry->value, properties));
        }
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_scalar(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                     const struct kefir_ast_type *type, const struct kefir_ast_initializer *initializer,
                                     struct kefir_ast_initializer_properties *properties) {
    struct kefir_ast_node_base *expr = kefir_ast_initializer_head(initializer);
    if (expr != NULL) {
        REQUIRE_OK(kefir_ast_node_assignable(mem, context, expr, type));
    } else {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, NULL, "Scalar initializer list cannot be empty");
    }
    if (properties != NULL) {
        properties->type = type;
    }
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
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected string literal type");
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_aggregate_union_string_literal(struct kefir_mem *mem,
                                                              const struct kefir_ast_context *context,
                                                              struct kefir_ast_type_traversal *traversal,
                                                              struct kefir_ast_initializer_list_entry *entry) {
    const struct kefir_ast_type *type = NULL;
    kefir_bool_t (*stop_fn)(const struct kefir_ast_type *, void *) = NULL;
    void *stop_payload = NULL;
    REQUIRE_OK(string_literal_stop_fn(entry->value->expression, context->type_traits, &stop_fn, &stop_payload));

    REQUIRE_OK(kefir_ast_type_traversal_next_recursive2(mem, traversal, stop_fn, stop_payload, &type, NULL));
    if (!stop_fn(type, stop_payload)) {
        REQUIRE_OK(kefir_ast_node_assignable(mem, context, entry->value->expression, kefir_ast_unqualified_type(type)));
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_aggregate_union(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                               const struct kefir_ast_initializer *initializer,
                                               struct kefir_ast_type_traversal *traversal) {
    const struct kefir_list_entry *init_iter = kefir_list_head(&initializer->list.initializers);
    for (; init_iter != NULL; kefir_list_next(&init_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, entry, init_iter->value);
        if (entry->designator != NULL) {
            REQUIRE_OK(kefir_ast_type_traversal_navigate(mem, traversal, entry->designator));
        } else if (kefir_ast_type_traversal_empty(traversal)) {
            continue;
        }

        if (entry->value->type == KEFIR_AST_INITIALIZER_LIST) {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, NULL));
            REQUIRE_OK(kefir_ast_analyze_initializer(mem, context, type, entry->value, NULL));
        } else if (entry->value->expression->properties.expression_props.string_literal.content != NULL) {
            REQUIRE_OK(traverse_aggregate_union_string_literal(mem, context, traversal, entry));
        } else if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(entry->value->expression->properties.type)) {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next_recursive(mem, traversal, &type, NULL));
            REQUIRE_OK(
                kefir_ast_node_assignable(mem, context, entry->value->expression, kefir_ast_unqualified_type(type)));
        } else {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, NULL));
            kefir_result_t res =
                kefir_ast_node_assignable(mem, context, entry->value->expression, kefir_ast_unqualified_type(type));
            while (res == KEFIR_NO_MATCH) {
                REQUIRE_OK(kefir_ast_type_traversal_step(mem, traversal));
                REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, NULL));
                res =
                    kefir_ast_node_assignable(mem, context, entry->value->expression, kefir_ast_unqualified_type(type));
            }
            REQUIRE_OK(res);
        }
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_struct_union(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                           const struct kefir_ast_type *type,
                                           const struct kefir_ast_initializer *initializer,
                                           struct kefir_ast_initializer_properties *properties) {
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, NULL, "Cannot initialize incomplete object type"));
    if (initializer->type == KEFIR_AST_INITIALIZER_EXPRESSION) {
        REQUIRE_OK(kefir_ast_node_assignable(mem, context, initializer->expression, type));
    } else {
        struct kefir_ast_type_traversal traversal;
        REQUIRE_OK(kefir_ast_type_traversal_init(mem, &traversal, type));
        kefir_result_t res = traverse_aggregate_union(mem, context, initializer, &traversal);
        REQUIRE_OK(kefir_ast_type_traversal_free(mem, &traversal));
        REQUIRE_OK(res);
    }
    if (properties != NULL) {
        properties->type = type;
    }
    return KEFIR_OK;
}

static kefir_result_t array_layer_next(const struct kefir_ast_type_traversal *traversal,
                                       const struct kefir_ast_type_traversal_layer *layer, void *payload) {
    UNUSED(traversal);
    REQUIRE(layer != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid type traversal layer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid type traversal listener payload"));
    ASSIGN_DECL_CAST(kefir_size_t *, array_length, payload)
    if (layer->parent == NULL) {
        REQUIRE(layer->type == KEFIR_AST_TYPE_TRAVERSAL_ARRAY,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected top traversal level type to be array"));
        *array_length = MAX(*array_length, layer->array.index + 1);
    }
    return KEFIR_OK;
}

static kefir_result_t array_layer_end(const struct kefir_ast_type_traversal *traversal,
                                      const struct kefir_ast_type_traversal_layer *layer, void *payload) {
    UNUSED(traversal);
    REQUIRE(layer != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid type traversal layer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid type traversal listener payload"));
    ASSIGN_DECL_CAST(kefir_size_t *, array_length, payload)
    if (layer->parent == NULL) {
        REQUIRE(layer->type == KEFIR_AST_TYPE_TRAVERSAL_ARRAY,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected top traversal level type to be array"));
        *array_length = MAX(*array_length, layer->array.index + (layer->init ? 0 : 1));
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_array(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                    const struct kefir_ast_type *type, const struct kefir_ast_initializer *initializer,
                                    struct kefir_ast_initializer_properties *properties) {
    struct kefir_ast_node_base *head_expr = kefir_ast_initializer_head(initializer);
    kefir_size_t array_length = 0;
    kefir_bool_t is_string = false;
    if (head_expr != NULL && head_expr->properties.expression_props.string_literal.content != NULL) {
        kefir_bool_t (*stop_fn)(const struct kefir_ast_type *, void *) = NULL;
        void *stop_payload = NULL;
        REQUIRE_OK(string_literal_stop_fn(head_expr, context->type_traits, &stop_fn, &stop_payload));
        if (stop_fn(type, stop_payload)) {
            array_length = head_expr->properties.expression_props.string_literal.length;
            is_string = true;
        }
    }

    if (!is_string) {
        REQUIRE(initializer->type == KEFIR_AST_INITIALIZER_LIST,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, NULL,
                                       "Unable to initialize array by non-string literal expression"));
        struct kefir_ast_type_traversal traversal;
        REQUIRE_OK(kefir_ast_type_traversal_init(mem, &traversal, type));
        traversal.events.layer_next = array_layer_next;
        traversal.events.layer_end = array_layer_end;
        traversal.events.payload = &array_length;
        kefir_result_t res = traverse_aggregate_union(mem, context, initializer, &traversal);
        REQUIRE_OK(kefir_ast_type_traversal_free(mem, &traversal));
        REQUIRE_OK(res);
    }
    if (properties != NULL) {
        if (type->array_type.boundary == KEFIR_AST_ARRAY_UNBOUNDED) {
            properties->type = kefir_ast_type_array(mem, context->type_bundle, type->array_type.element_type,
                                                    kefir_ast_constant_expression_integer(mem, array_length),
                                                    &type->array_type.qualifications);
        } else {
            properties->type = type;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_initializer(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                             const struct kefir_ast_type *type,
                                             const struct kefir_ast_initializer *initializer,
                                             struct kefir_ast_initializer_properties *properties) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST initializer"));

    type = kefir_ast_unqualified_type(type);

    if (properties != NULL) {
        properties->type = NULL;
        properties->constant = true;
    }
    REQUIRE_OK(preanalyze_initializer(mem, context, initializer, properties));
    if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(type)) {
        REQUIRE_OK(analyze_scalar(mem, context, type, initializer, properties));
    } else if (type->tag == KEFIR_AST_TYPE_ARRAY) {
        REQUIRE_OK(analyze_array(mem, context, type, initializer, properties));
    } else if (type->tag == KEFIR_AST_TYPE_STRUCTURE || type->tag == KEFIR_AST_TYPE_UNION) {
        REQUIRE_OK(analyze_struct_union(mem, context, type, initializer, properties));
    } else {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, NULL, "Cannot initialize incomplete object type");
    }
    return KEFIR_OK;
}
