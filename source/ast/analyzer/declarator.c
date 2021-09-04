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
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/function_declaration_context.h"

enum signedness { SIGNEDNESS_DEFAULT, SIGNEDNESS_SIGNED, SIGNEDNESS_UNSIGNED };

static struct kefir_ast_alignment *wrap_alignment(struct kefir_mem *mem, kefir_size_t alignment) {
    if (alignment > 0) {
        return kefir_ast_alignment_const_expression(mem, kefir_ast_constant_expression_integer(mem, alignment));
    } else {
        return NULL;
    }
}

static kefir_result_t process_struct_declaration_entry(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       struct kefir_ast_struct_type *struct_type,
                                                       struct kefir_ast_structure_declaration_entry *entry) {
    for (const struct kefir_list_entry *iter = kefir_list_head(&entry->declaration.declarators); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, entry_declarator, iter->value);

        const struct kefir_ast_type *field_type = NULL;
        kefir_ast_scoped_identifier_storage_t storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
        kefir_size_t alignment = 0;
        const char *identifier = NULL;
        REQUIRE_OK(kefir_ast_analyze_declaration(mem, context, &entry->declaration.specifiers,
                                                 entry_declarator->declarator, &identifier, &field_type, &storage_class,
                                                 NULL, &alignment));
        REQUIRE(storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Structure/union field cannot have storage class specified"));

        if (entry_declarator->bitwidth == NULL) {
            struct kefir_ast_alignment *ast_alignment = wrap_alignment(mem, alignment);
            REQUIRE(alignment == 0 || ast_alignment != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST alignment"));
            kefir_result_t res =
                kefir_ast_struct_type_field(mem, context->symbols, struct_type, identifier, field_type, ast_alignment);
            REQUIRE_ELSE(res == KEFIR_OK, {
                if (ast_alignment != NULL) {
                    kefir_ast_alignment_free(mem, ast_alignment);
                }
                return res;
            });
        } else {
            struct kefir_ast_constant_expression_value value;
            REQUIRE_OK(kefir_ast_analyze_node(mem, context, entry_declarator->bitwidth));
            REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, entry_declarator->bitwidth, &value));
            REQUIRE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bit-field width shall be integral constant expression"));
            struct kefir_ast_alignment *ast_alignment = wrap_alignment(mem, alignment);
            REQUIRE(alignment == 0 || ast_alignment != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST alignment"));
            struct kefir_ast_constant_expression *ast_bitwidth =
                kefir_ast_constant_expression_integer(mem, value.integer);
            REQUIRE_ELSE(ast_bitwidth != NULL, {
                if (ast_alignment != NULL) {
                    kefir_ast_alignment_free(mem, ast_alignment);
                }
                return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST constant expression");
            });
            kefir_result_t res = kefir_ast_struct_type_bitfield(mem, context->symbols, struct_type, identifier,
                                                                field_type, ast_alignment, ast_bitwidth);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_constant_expression_free(mem, ast_bitwidth);
                if (ast_alignment != NULL) {
                    kefir_ast_alignment_free(mem, ast_alignment);
                }
                return res;
            });
        }
    }
    if (kefir_list_head(&entry->declaration.declarators) == NULL) {
        const struct kefir_ast_type *field_type = NULL;
        kefir_ast_scoped_identifier_storage_t storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
        kefir_size_t alignment = 0;
        REQUIRE_OK(kefir_ast_analyze_declaration(mem, context, &entry->declaration.specifiers, NULL, NULL, &field_type,
                                                 &storage_class, NULL, &alignment));
        REQUIRE(storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Structure/union field cannot have storage class specified"));
        REQUIRE((field_type->tag == KEFIR_AST_TYPE_STRUCTURE || field_type->tag == KEFIR_AST_TYPE_UNION) &&
                    field_type->structure_type.identifier == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Anonymous member shall be a structure/union without a tag"));
        struct kefir_ast_alignment *ast_alignment = wrap_alignment(mem, alignment);
        REQUIRE(alignment == 0 || ast_alignment != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST alignment"));
        kefir_result_t res =
            kefir_ast_struct_type_field(mem, context->symbols, struct_type, NULL, field_type, ast_alignment);
        REQUIRE_ELSE(res == KEFIR_OK, {
            if (ast_alignment != NULL) {
                kefir_ast_alignment_free(mem, ast_alignment);
            }
            return res;
        });
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_struct_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                          kefir_ast_type_specifier_type_t specifier_type,
                                          const struct kefir_ast_structure_specifier *specifier,
                                          const struct kefir_ast_type **base_type) {
    kefir_bool_t resolved = false;
    const struct kefir_ast_type *type = NULL;
    if (specifier->complete) {
        struct kefir_ast_struct_type *struct_type = NULL;
        type = specifier_type == KEFIR_AST_TYPE_SPECIFIER_STRUCT
                   ? kefir_ast_type_structure(mem, context->type_bundle, specifier->identifier, &struct_type)
                   : kefir_ast_type_union(mem, context->type_bundle, specifier->identifier, &struct_type);
        REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Unable to allocate AST struct/union type"));

        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->entries); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, iter->value);
            if (entry->is_static_assertion) {
                REQUIRE_OK(kefir_ast_analyze_node(mem, context, KEFIR_AST_NODE_BASE(entry->static_assertion)));
            } else {
                REQUIRE_OK(process_struct_declaration_entry(mem, context, struct_type, entry));
            }
        }
    } else {
        if (specifier->identifier != NULL) {
            const struct kefir_ast_scoped_identifier *scoped_identifier = NULL;
            kefir_result_t res = context->resolve_tag_identifier(context, specifier->identifier, &scoped_identifier);
            if (res == KEFIR_OK) {
                REQUIRE((specifier_type == KEFIR_AST_TYPE_SPECIFIER_STRUCT &&
                         scoped_identifier->type->tag == KEFIR_AST_TYPE_STRUCTURE) ||
                            (specifier_type == KEFIR_AST_TYPE_SPECIFIER_UNION &&
                             scoped_identifier->type->tag == KEFIR_AST_TYPE_UNION),
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Tagged type declaration mismatch"));
                type = scoped_identifier->type;
                resolved = true;
            } else {
                REQUIRE(res == KEFIR_NOT_FOUND, res);
            }
        }

        if (!resolved) {
            type = specifier_type == KEFIR_AST_TYPE_SPECIFIER_STRUCT
                       ? kefir_ast_type_incomplete_structure(mem, context->type_bundle, specifier->identifier)
                       : kefir_ast_type_incomplete_union(mem, context->type_bundle, specifier->identifier);
            REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Unable to allocate AST struct/union type"));
        }
    }

    if (specifier->identifier != NULL && !resolved) {
        REQUIRE_OK(context->define_tag(mem, context, type));
    }
    ASSIGN_PTR(base_type, type);
    return KEFIR_OK;
}

static kefir_result_t resolve_enum_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                        const struct kefir_ast_enum_specifier *specifier,
                                        const struct kefir_ast_type **base_type) {
    kefir_bool_t resolved = false;
    const struct kefir_ast_type *type = NULL;
    if (specifier->complete) {
        struct kefir_ast_enum_type *enum_type = NULL;
        type = kefir_ast_type_enumeration(mem, context->type_bundle, specifier->identifier,
                                          context->type_traits->underlying_enumeration_type, &enum_type);
        REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Unable to allocate AST enum type"));

        kefir_ast_constant_expression_int_t constant_value = 0;
        for (const struct kefir_list_entry *iter = kefir_list_head(&specifier->entries); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier_entry *, entry, iter->value);

            if (entry->value != NULL) {
                struct kefir_ast_constant_expression_value value;
                REQUIRE_OK(kefir_ast_analyze_node(mem, context, entry->value));
                REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, entry->value, &value));
                REQUIRE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Enumeration constant valud shall be an integer constant expression"));
                constant_value = value.integer;
                REQUIRE_OK(
                    kefir_ast_enumeration_type_constant(mem, context->symbols, enum_type, entry->constant,
                                                        kefir_ast_constant_expression_integer(mem, constant_value)));
            } else {
                REQUIRE_OK(kefir_ast_enumeration_type_constant_auto(mem, context->symbols, enum_type, entry->constant));
            }
            REQUIRE_OK(context->define_constant(mem, context, entry->constant,
                                                kefir_ast_constant_expression_integer(mem, constant_value++),
                                                context->type_traits->underlying_enumeration_type));
        }
    } else {
        if (specifier->identifier != NULL) {
            const struct kefir_ast_scoped_identifier *scoped_identifier = NULL;
            kefir_result_t res = context->resolve_tag_identifier(context, specifier->identifier, &scoped_identifier);
            if (res == KEFIR_OK) {
                REQUIRE(scoped_identifier->type->tag == KEFIR_AST_TYPE_ENUMERATION,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Tagged type declaration mismatch"));
                type = scoped_identifier->type;
                resolved = true;
            } else {
                REQUIRE(res == KEFIR_NOT_FOUND, res);
            }
        }

        if (!resolved) {
            type = kefir_ast_type_incomplete_enumeration(mem, context->type_bundle, specifier->identifier,
                                                         context->type_traits->underlying_enumeration_type);
            REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Unable to allocate AST enum type"));
        }
    }

    if (specifier->identifier != NULL && !resolved) {
        REQUIRE_OK(context->define_tag(mem, context, type));
    }
    ASSIGN_PTR(base_type, type);
    return KEFIR_OK;
}

static kefir_result_t resolve_typedef(const struct kefir_ast_context *context, const char *type_name,
                                      const struct kefir_ast_type **base_type) {
    const struct kefir_ast_scoped_identifier *scoped_identifier = NULL;
    REQUIRE_OK(context->resolve_ordinary_identifier(context, type_name, &scoped_identifier));
    REQUIRE(scoped_identifier->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Referenced identifier is not a type definition"));
    *base_type = scoped_identifier->type;
    return KEFIR_OK;
}

enum type_specifier_sequence_state {
    TYPE_SPECIFIER_SEQUENCE_EMPTY,
    TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
    TYPE_SPECIFIER_SEQUENCE_SPECIFIERS
} type_specifier_sequence_state_t;

static kefir_result_t resolve_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   enum signedness *signedness, enum type_specifier_sequence_state *seq_state,
                                   const struct kefir_ast_type **base_type,
                                   const struct kefir_ast_type_specifier *specifier) {
    switch (specifier->specifier) {
        case KEFIR_AST_TYPE_SPECIFIER_VOID:
            REQUIRE(*base_type == NULL,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Void type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_void();
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_CHAR:
            REQUIRE(*base_type == NULL,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Char type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_char();
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_SHORT:
            REQUIRE(*base_type == NULL || (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_INT,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Short type specifier can only be combined with int"));
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            *base_type = kefir_ast_type_signed_short();
            break;

        case KEFIR_AST_TYPE_SPECIFIER_INT:
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            if (*base_type == NULL) {
                *base_type = kefir_ast_type_signed_int();
            } else {
                REQUIRE(
                    (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT ||
                        (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG ||
                        (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Int type specifier can only be combined with short or long"));
            }
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_LONG:
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            if (*base_type != NULL && (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG) {
                *base_type = kefir_ast_type_signed_long_long();
            } else {
                REQUIRE(
                    (*base_type) == NULL || (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_INT,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Long type specifier can only be combined with int or long"));
                *base_type = kefir_ast_type_signed_long();
            }
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_FLOAT:
            REQUIRE(*base_type == NULL,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Float type specifier cannot be combined with others"));
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            *base_type = kefir_ast_type_float();
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_DOUBLE:
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            if ((*base_type) == NULL) {
                *base_type = kefir_ast_type_double();
            } else {
                REQUIRE((*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Double type specifier can only be combined with complex and long"));
                return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Long and complex doubles are not supported yet");
            }
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_SIGNED:
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            REQUIRE(*signedness == SIGNEDNESS_DEFAULT,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                    "Signed type specifier cannot be combined with other signedness specifiers"));
            *signedness = SIGNEDNESS_SIGNED;
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_UNSIGNED:
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            REQUIRE(*signedness == SIGNEDNESS_DEFAULT,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                    "Unsigned type specifier cannot be combined with other signedness specifiers"));
            *signedness = SIGNEDNESS_UNSIGNED;
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_BOOL:
            REQUIRE(
                *seq_state != TYPE_SPECIFIER_SEQUENCE_TYPEDEF,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine type specifiers with referenced type definition"));
            REQUIRE(*base_type == NULL,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Boolean type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_bool();
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_COMPLEX:
            return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Complex floating-point type specifier is not supported yet");

        case KEFIR_AST_TYPE_SPECIFIER_ATOMIC:
            return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Atomic type specifier is not supported yet");

        case KEFIR_AST_TYPE_SPECIFIER_STRUCT:
        case KEFIR_AST_TYPE_SPECIFIER_UNION:
            REQUIRE(*seq_state == TYPE_SPECIFIER_SEQUENCE_EMPTY,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine struct/union type specifier with others"));
            REQUIRE_OK(resolve_struct_type(mem, context, specifier->specifier, specifier->value.structure, base_type));
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_ENUM:
            REQUIRE(*seq_state == TYPE_SPECIFIER_SEQUENCE_EMPTY,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine struct/union type specifier with others"));
            REQUIRE_OK(resolve_enum_type(mem, context, specifier->value.enumeration, base_type));
            *seq_state = TYPE_SPECIFIER_SEQUENCE_SPECIFIERS;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_TYPEDEF:
            REQUIRE(*seq_state == TYPE_SPECIFIER_SEQUENCE_EMPTY,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot combine referenced type definition with others"));
            REQUIRE_OK(resolve_typedef(context, specifier->value.type_name, base_type));
            *seq_state = TYPE_SPECIFIER_SEQUENCE_TYPEDEF;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type specifier");
    }
    return KEFIR_OK;
}

static kefir_result_t apply_type_signedness(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                            enum signedness signedness, const struct kefir_ast_type **base_type) {
    UNUSED(mem);
    UNUSED(type_bundle);
    if (signedness == SIGNEDNESS_DEFAULT) {
        if ((*base_type) == NULL) {
            (*base_type) = kefir_ast_type_signed_int();
        }
    } else if (signedness == SIGNEDNESS_SIGNED) {
        if ((*base_type) == NULL) {
            (*base_type) = kefir_ast_type_signed_int();
        } else {
            switch ((*base_type)->tag) {
                case KEFIR_AST_TYPE_SCALAR_CHAR:
                    (*base_type) = kefir_ast_type_signed_char();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
                case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
                    // Nothing to be done
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type");

                case KEFIR_AST_TYPE_VOID:
                case KEFIR_AST_TYPE_SCALAR_BOOL:
                case KEFIR_AST_TYPE_SCALAR_FLOAT:
                case KEFIR_AST_TYPE_SCALAR_DOUBLE:
                case KEFIR_AST_TYPE_SCALAR_POINTER:
                case KEFIR_AST_TYPE_ENUMERATION:
                case KEFIR_AST_TYPE_STRUCTURE:
                case KEFIR_AST_TYPE_UNION:
                case KEFIR_AST_TYPE_ARRAY:
                case KEFIR_AST_TYPE_FUNCTION:
                case KEFIR_AST_TYPE_QUALIFIED:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Signed type specifier cannot be applied to the type");
            }
        }
    } else {
        if ((*base_type) == NULL) {
            (*base_type) = kefir_ast_type_unsigned_int();
        } else {
            switch ((*base_type)->tag) {
                case KEFIR_AST_TYPE_SCALAR_CHAR:
                    (*base_type) = kefir_ast_type_unsigned_char();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
                    (*base_type) = kefir_ast_type_unsigned_short();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
                    (*base_type) = kefir_ast_type_unsigned_int();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
                    (*base_type) = kefir_ast_type_unsigned_long();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
                    (*base_type) = kefir_ast_type_unsigned_long_long();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type");

                case KEFIR_AST_TYPE_VOID:
                case KEFIR_AST_TYPE_SCALAR_BOOL:
                case KEFIR_AST_TYPE_SCALAR_FLOAT:
                case KEFIR_AST_TYPE_SCALAR_DOUBLE:
                case KEFIR_AST_TYPE_SCALAR_POINTER:
                case KEFIR_AST_TYPE_ENUMERATION:
                case KEFIR_AST_TYPE_STRUCTURE:
                case KEFIR_AST_TYPE_UNION:
                case KEFIR_AST_TYPE_ARRAY:
                case KEFIR_AST_TYPE_FUNCTION:
                case KEFIR_AST_TYPE_QUALIFIED:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                           "Unsigned type specifier cannot be applied to the type");
            }
        }
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_qualification(kefir_ast_type_qualifier_type_t qualifier,
                                            struct kefir_ast_type_qualification *qualifiers) {
    switch (qualifier) {
        case KEFIR_AST_TYPE_QUALIFIER_CONST:
            qualifiers->constant = true;
            break;

        case KEFIR_AST_TYPE_QUALIFIER_RESTRICT:
            qualifiers->restricted = true;
            break;

        case KEFIR_AST_TYPE_QUALIFIER_VOLATILE:
            qualifiers->volatile_type = true;
            break;

        case KEFIR_AST_TYPE_QUALIFIER_ATOMIC:
            return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Atomic types are not supported yet");

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type qualifier");
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_storage_class(kefir_ast_storage_class_specifier_type_t specifier,
                                            kefir_ast_scoped_identifier_storage_t *storage_class) {
    switch (specifier) {
        case KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF:
            REQUIRE(*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Typedef storage class cannot be combined with others"));
            *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF;
            break;

        case KEFIR_AST_STORAGE_SPECIFIER_EXTERN:
            if (*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN) {
                *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN;
            } else {
                REQUIRE(*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Extern storage class can only be colocated with thread_local"));
                *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL;
            }
            break;

        case KEFIR_AST_STORAGE_SPECIFIER_STATIC:
            if (*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN) {
                *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC;
            } else {
                REQUIRE(*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Static storage class can only be colocated with thread_local"));
                *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL;
            }
            break;

        case KEFIR_AST_STORAGE_SPECIFIER_THREAD_LOCAL:
            if (*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN) {
                *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL;
            } else if (*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC) {
                *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL;
            } else {
                REQUIRE(*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Thread_local storage class can only be colocated with extern or static"));
                *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL;
            }
            break;

        case KEFIR_AST_STORAGE_SPECIFIER_AUTO:
            REQUIRE(*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Auto storage class cannot be combined with others"));
            *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO;
            break;

        case KEFIR_AST_STORAGE_SPECIFIER_REGISTER:
            REQUIRE(*storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Register storage class cannot be combined with others"));
            *storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Invalid storage-class specifier");
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_function_specifier(kefir_ast_function_specifier_type_t specifier,
                                                 kefir_ast_function_specifier_t *function_specifier) {
    switch (specifier) {
        case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_NORETURN:
            *function_specifier =
                kefir_ast_context_merge_function_specifiers(*function_specifier, KEFIR_AST_FUNCTION_SPECIFIER_NORETURN);
            break;

        case KEFIR_AST_FUNCTION_SPECIFIER_TYPE_INLINE:
            *function_specifier =
                kefir_ast_context_merge_function_specifiers(*function_specifier, KEFIR_AST_FUNCTION_SPECIFIER_INLINE);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Invalid storage-class specifier");
    }
    return KEFIR_OK;
}

static kefir_result_t type_alignment(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                     const struct kefir_ast_type *type, kefir_size_t *alignment) {

    kefir_ast_target_environment_opaque_type_t target_type;
    struct kefir_ast_target_environment_object_info object_info;
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_GET_TYPE(mem, context->target_env, type, &target_type));
    kefir_result_t res =
        KEFIR_AST_TARGET_ENVIRONMENT_OBJECT_INFO(mem, context->target_env, target_type, NULL, &object_info);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, target_type);
        return res;
    });
    REQUIRE_OK(KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE(mem, context->target_env, target_type));
    *alignment = object_info.alignment;
    return KEFIR_OK;
}

static kefir_result_t evaluate_alignment(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         struct kefir_ast_node_base *node, kefir_size_t *alignment) {
    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node));
    if (node->klass->type == KEFIR_AST_TYPE_NAME) {
        kefir_size_t new_alignment = 0;
        REQUIRE_OK(type_alignment(mem, context, node->properties.type, &new_alignment));
        *alignment = MAX(*alignment, new_alignment);
    } else {
        struct kefir_ast_constant_expression_value value;
        REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, node, &value));
        REQUIRE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected alignment specifier to produce"));
        *alignment = MAX(*alignment, (kefir_size_t) value.integer);
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_pointer_declarator(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                 const struct kefir_ast_declarator *declarator,
                                                 const struct kefir_ast_type **base_type) {
    *base_type = kefir_ast_type_pointer(mem, context->type_bundle, *base_type);
    REQUIRE(*base_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST pointer type"));

    struct kefir_ast_type_qualification qualification = {false};
    kefir_ast_type_qualifier_type_t qualifier;
    for (const struct kefir_list_entry *iter =
             kefir_ast_type_qualifier_list_iter(&declarator->pointer.type_qualifiers, &qualifier);
         iter != NULL; kefir_ast_type_qualifier_list_next(&iter, &qualifier)) {
        REQUIRE_OK(resolve_qualification(qualifier, &qualification));
    }
    if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualification)) {
        *base_type = kefir_ast_type_qualified(mem, context->type_bundle, *base_type, qualification);
        REQUIRE(*base_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST qualified type"));
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_array_declarator(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                               const struct kefir_ast_declarator *declarator,
                                               const struct kefir_ast_type **base_type) {
    struct kefir_ast_type_qualification qualification = {false};
    kefir_ast_type_qualifier_type_t qualifier;
    for (const struct kefir_list_entry *iter =
             kefir_ast_type_qualifier_list_iter(&declarator->array.type_qualifiers, &qualifier);
         iter != NULL; kefir_ast_type_qualifier_list_next(&iter, &qualifier)) {
        REQUIRE_OK(resolve_qualification(qualifier, &qualification));
    }

    switch (declarator->array.type) {
        case KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED:
            *base_type = kefir_ast_type_unbounded_array(mem, context->type_bundle, *base_type, &qualification);
            REQUIRE(*base_type != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST unbounded array type"));
            break;

        case KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED:
            if (declarator->array.static_array) {
                *base_type =
                    kefir_ast_type_vlen_array_static(mem, context->type_bundle, *base_type, NULL, &qualification);
            } else {
                *base_type = kefir_ast_type_vlen_array(mem, context->type_bundle, *base_type, NULL, &qualification);
            }
            REQUIRE(*base_type != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST unbounded array type"));
            break;

        case KEFIR_AST_DECLARATOR_ARRAY_BOUNDED: {
            struct kefir_ast_constant_expression_value value;
            REQUIRE_OK(kefir_ast_analyze_node(mem, context, declarator->array.length));
            kefir_result_t res =
                kefir_ast_constant_expression_value_evaluate(mem, context, declarator->array.length, &value);
            if (res == KEFIR_NOT_CONSTANT) {
                if (declarator->array.static_array) {
                    *base_type = kefir_ast_type_vlen_array_static(mem, context->type_bundle, *base_type,
                                                                  KEFIR_AST_NODE_CLONE(mem, declarator->array.length),
                                                                  &qualification);
                } else {
                    *base_type =
                        kefir_ast_type_vlen_array(mem, context->type_bundle, *base_type,
                                                  KEFIR_AST_NODE_CLONE(mem, declarator->array.length), &qualification);
                }
            } else {
                REQUIRE_OK(res);
                REQUIRE(value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Constant length of AST array declaration shall have integral type"));
                if (declarator->array.static_array) {
                    *base_type = kefir_ast_type_array_static(mem, context->type_bundle, *base_type,
                                                             kefir_ast_constant_expression_integer(mem, value.integer),
                                                             &qualification);
                } else {
                    *base_type =
                        kefir_ast_type_array(mem, context->type_bundle, *base_type,
                                             kefir_ast_constant_expression_integer(mem, value.integer), &qualification);
                }
            }
        } break;
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_function_declarator(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_declarator *declarator,
                                                  const struct kefir_ast_type **base_type) {
    const char *identifier = NULL;
    if (declarator->function.declarator != NULL &&
        declarator->function.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER) {
        identifier = declarator->function.declarator->identifier;
    }

    struct kefir_ast_function_type *func_type = NULL;
    const struct kefir_ast_type *type =
        kefir_ast_type_function(mem, context->type_bundle, *base_type, identifier, &func_type);
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocated AST type"));

    struct kefir_ast_function_declaration_context *decl_context =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_function_declaration_context));
    REQUIRE(decl_context != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST function declaration context"));
    kefir_result_t res = kefir_ast_function_declaration_context_init(mem, context, decl_context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl_context);
        return res;
    });

    res = KEFIR_OK;
    for (const struct kefir_list_entry *iter = kefir_list_head(&declarator->function.parameters);
         iter != NULL && res == KEFIR_OK; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, iter->value);

        if (node->klass->type == KEFIR_AST_DECLARATION) {
            ASSIGN_DECL_CAST(struct kefir_ast_declaration *, decl_list, node->self);
            struct kefir_ast_init_declarator *declaration = NULL;
            res = kefir_ast_analyze_node(mem, &decl_context->context, node);
            REQUIRE_CHAIN(&res, kefir_ast_declaration_unpack_single(decl_list, &declaration));
            REQUIRE_CHAIN(&res,
                          kefir_ast_type_function_parameter(mem, context->type_bundle, func_type,
                                                            declaration->base.properties.declaration_props.identifier,
                                                            declaration->base.properties.type,
                                                            &declaration->base.properties.declaration_props.storage));
        } else if (node->klass->type == KEFIR_AST_IDENTIFIER) {
            res = kefir_ast_analyze_node(mem, &decl_context->context, node);
            if (res == KEFIR_OK) {
                if (node->properties.category != KEFIR_AST_NODE_CATEGORY_TYPE) {
                    res = KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                          "Function declaration parameter shall be either declaration, or identifier");
                }
                REQUIRE_CHAIN(&res, kefir_ast_type_function_parameter(mem, context->type_bundle, func_type, NULL,
                                                                      node->properties.type, NULL));
            } else if (res == KEFIR_NOT_FOUND) {
                res = KEFIR_OK;
                ASSIGN_DECL_CAST(struct kefir_ast_identifier *, ident, node->self);
                res = kefir_ast_type_function_parameter(mem, context->type_bundle, func_type, ident->identifier, NULL,
                                                        NULL);
            }
        } else {
            res = KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                  "Function declaration parameter shall be either declaration, or identifier");
        }
    }

    REQUIRE_CHAIN(&res, kefir_ast_type_function_ellipsis(func_type, declarator->function.ellipsis));

    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_function_declaration_context_free(mem, decl_context);
        KEFIR_FREE(mem, decl_context);
        return res;
    });

    res = kefir_list_insert_after(mem, context->function_decl_contexts,
                                  kefir_list_tail(context->function_decl_contexts), decl_context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_function_declaration_context_free(mem, decl_context);
        KEFIR_FREE(mem, decl_context);
        return res;
    });

    *base_type = type;
    return KEFIR_OK;
}

static kefir_result_t resolve_declarator(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_declarator *declarator, const char **identifier,
                                         const struct kefir_ast_type **base_type) {
    ASSIGN_PTR(identifier, NULL);
    REQUIRE(declarator != NULL, KEFIR_OK);
    switch (declarator->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            ASSIGN_PTR(identifier, declarator->identifier);
            break;

        case KEFIR_AST_DECLARATOR_POINTER:
            REQUIRE_OK(resolve_pointer_declarator(mem, context, declarator, base_type));
            REQUIRE_OK(resolve_declarator(mem, context, declarator->pointer.declarator, identifier, base_type));
            break;

        case KEFIR_AST_DECLARATOR_ARRAY:
            REQUIRE_OK(resolve_array_declarator(mem, context, declarator, base_type));
            REQUIRE_OK(resolve_declarator(mem, context, declarator->array.declarator, identifier, base_type));
            break;

        case KEFIR_AST_DECLARATOR_FUNCTION:
            REQUIRE_OK(resolve_function_declarator(mem, context, declarator, base_type));
            REQUIRE_OK(resolve_declarator(mem, context, declarator->function.declarator, identifier, base_type));
            break;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_declaration(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                             const struct kefir_ast_declarator_specifier_list *specifiers,
                                             const struct kefir_ast_declarator *declarator, const char **identifier,
                                             const struct kefir_ast_type **type,
                                             kefir_ast_scoped_identifier_storage_t *storage,
                                             kefir_ast_function_specifier_t *function, kefir_size_t *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(specifiers != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declarator specifier list"));

    enum signedness signedness = SIGNEDNESS_DEFAULT;
    enum type_specifier_sequence_state seq_state = TYPE_SPECIFIER_SEQUENCE_EMPTY;
    const struct kefir_ast_type *base_type = NULL;
    struct kefir_ast_type_qualification qualification = {false};
    kefir_ast_scoped_identifier_storage_t storage_class = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_ast_function_specifier_t function_specifier = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
    kefir_size_t alignment_specifier = 0;

    struct kefir_ast_declarator_specifier *declatator_specifier;
    for (struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(specifiers, &declatator_specifier);
         iter != NULL; kefir_ast_declarator_specifier_list_next(&iter, &declatator_specifier)) {
        switch (declatator_specifier->klass) {
            case KEFIR_AST_TYPE_SPECIFIER:
                REQUIRE_OK(resolve_type(mem, context, &signedness, &seq_state, &base_type,
                                        &declatator_specifier->type_specifier));
                break;

            case KEFIR_AST_TYPE_QUALIFIER:
                REQUIRE_OK(resolve_qualification(declatator_specifier->type_qualifier, &qualification));
                break;

            case KEFIR_AST_STORAGE_CLASS_SPECIFIER:
                REQUIRE_OK(resolve_storage_class(declatator_specifier->storage_class, &storage_class));
                break;

            case KEFIR_AST_FUNCTION_SPECIFIER:
                REQUIRE_OK(resolve_function_specifier(declatator_specifier->function_specifier, &function_specifier));
                break;

            case KEFIR_AST_ALIGNMENT_SPECIFIER:
                REQUIRE_OK(
                    evaluate_alignment(mem, context, declatator_specifier->alignment_specifier, &alignment_specifier));
                break;
        }
    }
    REQUIRE_OK(apply_type_signedness(mem, context->type_bundle, signedness, &base_type));
    if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualification)) {
        base_type = kefir_ast_type_qualified(mem, context->type_bundle, base_type, qualification);
    }

    REQUIRE_OK(resolve_declarator(mem, context, declarator, identifier, &base_type));
    ASSIGN_PTR(type, base_type);
    ASSIGN_PTR(storage, storage_class);
    ASSIGN_PTR(function, function_specifier);

    if (alignment != NULL) {
        if (alignment_specifier > 0) {
            kefir_size_t natural_alignment = 0;
            REQUIRE_OK(type_alignment(mem, context, base_type, &natural_alignment));
            REQUIRE(natural_alignment <= alignment_specifier,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Specified alignment shall be at least as strict as natural"));
            *alignment = alignment_specifier;
        } else {
            *alignment = 0;
        }
    }
    return KEFIR_OK;
}
