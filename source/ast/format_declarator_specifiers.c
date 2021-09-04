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

static kefir_result_t format_struct(struct kefir_json_output *json,
                                    const struct kefir_ast_declarator_specifier *specifier,
                                    kefir_bool_t display_source_location) {
    if (specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_STRUCT) {
        REQUIRE_OK(kefir_json_output_string(json, "struct"));
    } else {
        REQUIRE_OK(kefir_json_output_string(json, "union"));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    if (specifier->type_specifier.value.structure->identifier != NULL) {
        REQUIRE_OK(kefir_json_output_string(json, specifier->type_specifier.value.structure->identifier));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "members"));
    if (specifier->type_specifier.value.structure->complete) {
        REQUIRE_OK(kefir_json_output_array_begin(json));
        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->type_specifier.value.structure->entries);
             iter != NULL; kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, iter->value);

            REQUIRE_OK(kefir_json_output_object_begin(json));
            REQUIRE_OK(kefir_json_output_object_key(json, "type"));
            if (entry->is_static_assertion) {
                REQUIRE_OK(kefir_json_output_string(json, "static_assert"));
                REQUIRE_OK(kefir_json_output_object_key(json, "assertion"));
                REQUIRE_OK(
                    kefir_ast_format(json, KEFIR_AST_NODE_BASE(entry->static_assertion), display_source_location));
            } else {
                REQUIRE_OK(kefir_json_output_string(json, "declaration"));
                REQUIRE_OK(kefir_json_output_object_key(json, "specifiers"));
                REQUIRE_OK(kefir_ast_format_declarator_specifier_list(json, &entry->declaration.specifiers,
                                                                      display_source_location));
                REQUIRE_OK(kefir_json_output_object_key(json, "declarators"));
                REQUIRE_OK(kefir_json_output_array_begin(json));
                for (const struct kefir_list_entry *iter2 = kefir_list_head(&entry->declaration.declarators);
                     iter2 != NULL; kefir_list_next(&iter2)) {
                    ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, declarator, iter2->value);
                    REQUIRE_OK(kefir_json_output_object_begin(json));
                    REQUIRE_OK(kefir_json_output_object_key(json, "declarator"));
                    REQUIRE_OK(kefir_ast_format_declarator(json, declarator->declarator, display_source_location));
                    REQUIRE_OK(kefir_json_output_object_key(json, "bitwidth"));
                    if (declarator->bitwidth != NULL) {
                        REQUIRE_OK(kefir_ast_format(json, declarator->bitwidth, display_source_location));
                    } else {
                        REQUIRE_OK(kefir_json_output_null(json));
                    }
                    REQUIRE_OK(kefir_json_output_object_end(json));
                }
                REQUIRE_OK(kefir_json_output_array_end(json));
            }
            REQUIRE_OK(kefir_json_output_object_end(json));
        }
        REQUIRE_OK(kefir_json_output_array_end(json));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    return KEFIR_OK;
}

static kefir_result_t format_enum(struct kefir_json_output *json,
                                  const struct kefir_ast_declarator_specifier *specifier,
                                  kefir_bool_t display_source_location) {
    REQUIRE_OK(kefir_json_output_string(json, "enum"));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    if (specifier->type_specifier.value.enumeration->identifier != NULL) {
        REQUIRE_OK(kefir_json_output_string(json, specifier->type_specifier.value.enumeration->identifier));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "members"));
    if (specifier->type_specifier.value.enumeration->complete) {
        REQUIRE_OK(kefir_json_output_array_begin(json));
        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->type_specifier.value.structure->entries);
             iter != NULL; kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);
            REQUIRE_OK(kefir_json_output_object_begin(json));
            REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
            REQUIRE_OK(kefir_json_output_string(json, entry->constant));
            REQUIRE_OK(kefir_json_output_object_key(json, "value"));
            if (entry->value != NULL) {
                REQUIRE_OK(kefir_ast_format(json, entry->value, display_source_location));
            } else {
                REQUIRE_OK(kefir_json_output_null(json));
            }
            REQUIRE_OK(kefir_json_output_object_end(json));
        }
        REQUIRE_OK(kefir_json_output_array_end(json));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_format_declarator_specifier(struct kefir_json_output *json,
                                                     const struct kefir_ast_declarator_specifier *specifier,
                                                     kefir_bool_t display_source_location) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator specifier"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "class"));
    switch (specifier->klass) {
        case KEFIR_AST_TYPE_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "type_specifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "specifier"));
            switch (specifier->type_specifier.specifier) {
                case KEFIR_AST_TYPE_SPECIFIER_VOID:
                    REQUIRE_OK(kefir_json_output_string(json, "void"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_CHAR:
                    REQUIRE_OK(kefir_json_output_string(json, "char"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_SHORT:
                    REQUIRE_OK(kefir_json_output_string(json, "short"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_INT:
                    REQUIRE_OK(kefir_json_output_string(json, "int"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_LONG:
                    REQUIRE_OK(kefir_json_output_string(json, "long"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_FLOAT:
                    REQUIRE_OK(kefir_json_output_string(json, "float"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_DOUBLE:
                    REQUIRE_OK(kefir_json_output_string(json, "double"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_SIGNED:
                    REQUIRE_OK(kefir_json_output_string(json, "signed"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_UNSIGNED:
                    REQUIRE_OK(kefir_json_output_string(json, "unsigned"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_BOOL:
                    REQUIRE_OK(kefir_json_output_string(json, "bool"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_COMPLEX:
                    REQUIRE_OK(kefir_json_output_string(json, "complex"));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ATOMIC:
                    REQUIRE_OK(kefir_json_output_string(json, "atomic"));
                    REQUIRE_OK(kefir_json_output_object_key(json, "atomic"));
                    REQUIRE_OK(
                        kefir_ast_format(json, specifier->type_specifier.value.atomic_type, display_source_location));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_STRUCT:
                case KEFIR_AST_TYPE_SPECIFIER_UNION:
                    REQUIRE_OK(format_struct(json, specifier, display_source_location));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ENUM:
                    REQUIRE_OK(format_enum(json, specifier, display_source_location));
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_TYPEDEF:
                    REQUIRE_OK(kefir_json_output_string(json, "typedef"));
                    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
                    REQUIRE_OK(kefir_json_output_string(json, specifier->type_specifier.value.type_name));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type specifier");
            }
            break;

        case KEFIR_AST_TYPE_QUALIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "type_qualifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "qualifier"));
            switch (specifier->type_qualifier) {
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
            break;

        case KEFIR_AST_STORAGE_CLASS_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "storage_class"));
            REQUIRE_OK(kefir_json_output_object_key(json, "storage"));
            switch (specifier->storage_class) {
                case KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF:
                    REQUIRE_OK(kefir_json_output_string(json, "typedef"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_EXTERN:
                    REQUIRE_OK(kefir_json_output_string(json, "extern"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_STATIC:
                    REQUIRE_OK(kefir_json_output_string(json, "static"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_THREAD_LOCAL:
                    REQUIRE_OK(kefir_json_output_string(json, "thread_local"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_AUTO:
                    REQUIRE_OK(kefir_json_output_string(json, "auto"));
                    break;

                case KEFIR_AST_STORAGE_SPECIFIER_REGISTER:
                    REQUIRE_OK(kefir_json_output_string(json, "register"));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected storage class");
            }
            break;

        case KEFIR_AST_FUNCTION_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "function_specifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "specifier"));
            switch (specifier->function_specifier) {
                case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_NORETURN:
                    REQUIRE_OK(kefir_json_output_string(json, "noreturn"));
                    break;

                case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_INLINE:
                    REQUIRE_OK(kefir_json_output_string(json, "inline"));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected function specifier");
            }
            break;

        case KEFIR_AST_ALIGNMENT_SPECIFIER:
            REQUIRE_OK(kefir_json_output_string(json, "alignment_specifier"));
            REQUIRE_OK(kefir_json_output_object_key(json, "alignment"));
            REQUIRE_OK(kefir_ast_format(json, specifier->alignment_specifier, display_source_location));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected declarator specifier");
    }
    REQUIRE_OK(kefir_json_output_object_end(json));

    return KEFIR_OK;
}

kefir_result_t kefir_ast_format_declarator_specifier_list(struct kefir_json_output *json,
                                                          const struct kefir_ast_declarator_specifier_list *specifiers,
                                                          kefir_bool_t display_source_location) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid JSON output"));
    REQUIRE(specifiers != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator specifier list"));

    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_ast_declarator_specifier *specifier = NULL;
    struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(specifiers, &specifier);
    kefir_result_t res = KEFIR_OK;
    for (; res == KEFIR_OK && iter != NULL; res = kefir_ast_declarator_specifier_list_next(&iter, &specifier)) {
        REQUIRE_OK(kefir_ast_format_declarator_specifier(json, specifier, display_source_location));
    }
    REQUIRE_OK(res);
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}
