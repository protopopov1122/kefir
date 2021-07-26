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

#include "kefir/ast/format.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t visit_not_impl(const struct kefir_ast_visitor *visitor, const struct kefir_ast_node_base *node,
                                     void *payload) {
    UNUSED(visitor);
    UNUSED(node);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "AST node JSON formatter is not implemented yet");
}

static kefir_result_t visit_identifier(const struct kefir_ast_visitor *visitor, const struct kefir_ast_identifier *node,
                                       void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST identifier node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "identifier"));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    REQUIRE_OK(kefir_json_output_string(json, node->identifier));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_constant(const struct kefir_ast_visitor *visitor, const struct kefir_ast_constant *node,
                                     void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "constant"));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    switch (node->type) {
        case KEFIR_AST_BOOL_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "boolean"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_boolean(json, node->value.boolean));
            break;

        case KEFIR_AST_CHAR_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "character"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.character));
            break;

        case KEFIR_AST_INT_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "integer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, node->value.integer));
            break;

        case KEFIR_AST_UINT_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned_integer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.uinteger));
            break;

        case KEFIR_AST_LONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_integer(json, node->value.long_integer));
            break;

        case KEFIR_AST_ULONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned_long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.ulong_integer));
            break;

        case KEFIR_AST_LONG_LONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "long_long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.long_long));
            break;

        case KEFIR_AST_ULONG_LONG_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "unsigned_long_long"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_uinteger(json, node->value.ulong_long));
            break;

        case KEFIR_AST_FLOAT_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "float"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_float(json, node->value.float32));
            break;

        case KEFIR_AST_DOUBLE_CONSTANT:
            REQUIRE_OK(kefir_json_output_string(json, "double"));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            REQUIRE_OK(kefir_json_output_float(json, node->value.float64));
            break;
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t visit_string_literal(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_string_literal *node, void *payload) {
    UNUSED(visitor);
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST string literal node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_json_output *, json, payload);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    REQUIRE_OK(kefir_json_output_string(json, "string_literal"));
    REQUIRE_OK(kefir_json_output_object_key(json, "content"));
    REQUIRE_OK(kefir_json_output_string(json, node->literal));
    REQUIRE_OK(kefir_json_output_object_key(json, "length"));
    REQUIRE_OK(kefir_json_output_uinteger(json, node->length));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_format(struct kefir_json_output *json, const struct kefir_ast_node_base *node) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node"));

    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_not_impl));
    visitor.identifier = visit_identifier;
    visitor.constant = visit_constant;
    visitor.string_literal = visit_string_literal;
    REQUIRE_OK(node->klass->visit(node, &visitor, json));
    return KEFIR_OK;
}
