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

#include "kefir/ast/format.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t format_type_qualifiers(struct kefir_json_output *json,
                                             const struct kefir_ast_type_qualifier_list *type_qualifiers) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    kefir_ast_type_qualifier_type_t qualifier;
    for (const struct kefir_list_entry *iter = kefir_ast_type_qualifier_list_iter(type_qualifiers, &qualifier);
         iter != NULL; kefir_ast_type_qualifier_list_next(&iter, &qualifier)) {
        switch (qualifier) {
            case KEFIR_AST_TYPE_QUALIFIER_CONST:
                REQUIRE_OK(kefir_json_output_string(json, "const"));
                break;

            case KEFIR_AST_TYPE_QUALIFIER_RESTRICT:
                REQUIRE_OK(kefir_json_output_string(json, "restrict"));
                break;

            case KEFIR_AST_TYPE_QUALIFIER_VOLATILE:
                REQUIRE_OK(kefir_json_output_string(json, "volatile"));
                break;

            case KEFIR_AST_TYPE_QUALIFIER_ATOMIC:
                REQUIRE_OK(kefir_json_output_string(json, "atomic"));
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type qualifier");
        }
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_format_declarator(struct kefir_json_output *json,
                                           const struct kefir_ast_declarator *declarator) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    if (declarator == NULL) {
        REQUIRE_OK(kefir_json_output_null(json));
        return KEFIR_OK;
    }

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    switch (declarator->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            if (declarator->identifier != NULL) {
                REQUIRE_OK(kefir_json_output_string(json, "identifier"));
                REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
                REQUIRE_OK(kefir_json_output_string(json, declarator->identifier));
            } else {
                REQUIRE_OK(kefir_json_output_string(json, "abstract"));
            }
            break;

        case KEFIR_AST_DECLARATOR_POINTER: {
            REQUIRE_OK(kefir_json_output_string(json, "pointer"));
            REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
            REQUIRE_OK(kefir_ast_format_declarator(json, declarator->pointer.declarator));
            REQUIRE_OK(kefir_json_output_object_key(json, "type_qualifiers"));
            REQUIRE_OK(format_type_qualifiers(json, &declarator->pointer.type_qualifiers));
        } break;

        case KEFIR_AST_DECLARATOR_ARRAY: {
            REQUIRE_OK(kefir_json_output_string(json, "array"));
            REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
            REQUIRE_OK(kefir_ast_format_declarator(json, declarator->array.declarator));
            REQUIRE_OK(kefir_json_output_object_key(json, "type"));
            switch (declarator->array.type) {
                case KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED:
                    REQUIRE_OK(kefir_json_output_string(json, "unbounded"));
                    break;

                case KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED:
                    REQUIRE_OK(kefir_json_output_string(json, "vla_unspecified"));
                    break;

                case KEFIR_AST_DECLARATOR_ARRAY_BOUNDED:
                    REQUIRE_OK(kefir_json_output_string(json, "bounded"));
                    REQUIRE_OK(kefir_json_output_object_key(json, "length"));
                    REQUIRE_OK(kefir_ast_format(json, declarator->array.length));
                    break;
            }
            REQUIRE_OK(kefir_json_output_object_key(json, "static"));
            REQUIRE_OK(kefir_json_output_boolean(json, declarator->array.static_array));
            REQUIRE_OK(kefir_json_output_object_key(json, "type_qualifiers"));
            REQUIRE_OK(format_type_qualifiers(json, &declarator->array.type_qualifiers));
        } break;

        case KEFIR_AST_DECLARATOR_FUNCTION: {
            REQUIRE_OK(kefir_json_output_string(json, "function"));
            REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
            REQUIRE_OK(kefir_ast_format_declarator(json, declarator->function.declarator));
            REQUIRE_OK(kefir_json_output_object_key(json, "ellipsis"));
            REQUIRE_OK(kefir_json_output_boolean(json, declarator->function.ellipsis));
            REQUIRE_OK(kefir_json_output_object_key(json, "parameters"));
            REQUIRE_OK(kefir_json_output_array_begin(json));
            for (const struct kefir_list_entry *iter = kefir_list_head(&declarator->function.parameters); iter != NULL;
                 kefir_list_next(&iter)) {
                ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, iter->value);
                REQUIRE_OK(kefir_ast_format(json, param));
            }
            REQUIRE_OK(kefir_json_output_array_end(json));
        } break;
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}
