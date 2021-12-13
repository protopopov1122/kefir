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

#include "kefir/ast/context.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

typedef struct kefir_ast_named_structure_resolver {
    kefir_result_t (*resolve)(const char *, const struct kefir_ast_type **, void *);
    void *payload;
} kefir_ast_named_structure_resolver_t;

static kefir_result_t resolve_named_structure(const char *identifier, const struct kefir_ast_type **type_ptr,
                                              void *payload) {
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST structure identifier"));
    REQUIRE(type_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST type"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(const struct kefir_ast_context *, context, payload);

    const struct kefir_ast_scoped_identifier *struct_scoped_id = NULL;
    REQUIRE_OK(context->resolve_tag_identifier(context, identifier, &struct_scoped_id));
    REQUIRE(struct_scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG &&
                (struct_scoped_id->type->tag == KEFIR_AST_TYPE_STRUCTURE ||
                 struct_scoped_id->type->tag == KEFIR_AST_TYPE_UNION) &&
                struct_scoped_id->type->structure_type.complete,
            KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Unable to find complete named structure type"));
    *type_ptr = struct_scoped_id->type;
    return KEFIR_OK;
}

static kefir_result_t kefir_ast_context_named_structure_resolver_init(
    const struct kefir_ast_context *context, struct kefir_ast_named_structure_resolver *resolver) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(resolver != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to named structure resolver"));

    resolver->resolve = resolve_named_structure;
    resolver->payload = (void *) context;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_completion(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_type **dst, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(dst != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to destination AST type"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source AST type"));

    switch (type->tag) {
        case KEFIR_AST_TYPE_VOID:
        case KEFIR_AST_TYPE_SCALAR_BOOL:
        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_FLOAT:
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
        case KEFIR_AST_TYPE_SCALAR_LONG_DOUBLE:
        case KEFIR_AST_TYPE_ENUMERATION:
        case KEFIR_AST_TYPE_FUNCTION:
        case KEFIR_AST_TYPE_VA_LIST:
            *dst = type;
            break;

        case KEFIR_AST_TYPE_SCALAR_POINTER: {
            const struct kefir_ast_type *referenced_type = NULL;
            REQUIRE_OK(kefir_ast_type_completion(mem, context, &referenced_type, type->referenced_type));
            *dst = kefir_ast_type_pointer(mem, context->type_bundle, referenced_type);
            REQUIRE(*dst != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST type"));
        } break;

        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION: {
            if (KEFIR_AST_TYPE_IS_INCOMPLETE(type) && type->structure_type.identifier != NULL) {
                struct kefir_ast_named_structure_resolver resolver;
                REQUIRE_OK(kefir_ast_context_named_structure_resolver_init(context, &resolver));
                kefir_result_t res = resolver.resolve(type->structure_type.identifier, dst, resolver.payload);
                if (res == KEFIR_NOT_FOUND) {
                    *dst = type;
                } else {
                    REQUIRE_OK(res);
                }
            } else {
                *dst = type;
            }
        } break;

        case KEFIR_AST_TYPE_ARRAY: {
            const struct kefir_ast_type *element_type = NULL;
            REQUIRE_OK(kefir_ast_type_completion(mem, context, &element_type, type->array_type.element_type));
            switch (type->array_type.boundary) {
                case KEFIR_AST_ARRAY_UNBOUNDED:
                    *dst = kefir_ast_type_unbounded_array(mem, context->type_bundle, element_type,
                                                          &type->array_type.qualifications);
                    REQUIRE(*dst != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST type"));
                    break;

                case KEFIR_AST_ARRAY_BOUNDED:
                case KEFIR_AST_ARRAY_BOUNDED_STATIC: {
                    struct kefir_ast_constant_expression *len =
                        kefir_ast_constant_expression_integer(mem, type->array_type.const_length->value.integer);
                    REQUIRE(len != NULL,
                            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST constant expression"));
                    if (type->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC) {
                        *dst = kefir_ast_type_array_static(mem, context->type_bundle, element_type, len,
                                                           &type->array_type.qualifications);
                    } else {
                        *dst = kefir_ast_type_array(mem, context->type_bundle, element_type, len,
                                                    &type->array_type.qualifications);
                    }
                    REQUIRE_ELSE(*dst != NULL, {
                        kefir_ast_constant_expression_free(mem, len);
                        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST type");
                    });
                } break;

                case KEFIR_AST_ARRAY_VLA:
                case KEFIR_AST_ARRAY_VLA_STATIC: {
                    struct kefir_ast_node_base *len = KEFIR_AST_NODE_CLONE(mem, type->array_type.vla_length);
                    REQUIRE(len != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to clone AST node"));
                    if (type->array_type.boundary == KEFIR_AST_ARRAY_VLA_STATIC) {
                        *dst = kefir_ast_type_vlen_array_static(mem, context->type_bundle, element_type, len,
                                                                &type->array_type.qualifications);
                    } else {
                        *dst = kefir_ast_type_vlen_array(mem, context->type_bundle, element_type, len,
                                                         &type->array_type.qualifications);
                    }
                    REQUIRE_ELSE(*dst != NULL, {
                        KEFIR_AST_NODE_FREE(mem, len);
                        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST type");
                    });
                } break;
            }
        } break;

        case KEFIR_AST_TYPE_QUALIFIED: {
            const struct kefir_ast_type *underlying_type = NULL;
            REQUIRE_OK(kefir_ast_type_completion(mem, context, &underlying_type, type->qualified_type.type));
            *dst = kefir_ast_type_qualified(mem, context->type_bundle, underlying_type,
                                            type->qualified_type.qualification);
            REQUIRE(*dst != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST type"));
        } break;
    }

    return KEFIR_OK;
}
