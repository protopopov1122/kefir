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

#include "kefir/ast/type.h"
#include "kefir/ast/node_base.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t same_array_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_ARRAY && type2->tag == KEFIR_AST_TYPE_ARRAY, false);
    REQUIRE(type1->array_type.boundary == type2->array_type.boundary, false);
    switch (type1->array_type.boundary) {
        case KEFIR_AST_ARRAY_UNBOUNDED:
            break;

        case KEFIR_AST_ARRAY_VLA:
        case KEFIR_AST_ARRAY_VLA_STATIC:
            REQUIRE((type1->array_type.vla_length == NULL && type2->array_type.vla_length == NULL) ||
                        (type1->array_type.vla_length != NULL && type2->array_type.vla_length != NULL),
                    false);
            break;

        case KEFIR_AST_ARRAY_BOUNDED:
        case KEFIR_AST_ARRAY_BOUNDED_STATIC:
            REQUIRE(type1->array_type.const_length->value.integer == type2->array_type.const_length->value.integer,
                    false);
            break;
    }
    return KEFIR_AST_TYPE_SAME(type1->array_type.element_type, type2->array_type.element_type);
}

static kefir_bool_t compatbile_array_types(const struct kefir_ast_type_traits *type_traits,
                                           const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_ARRAY && type2->tag == KEFIR_AST_TYPE_ARRAY, false);
    REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1->array_type.element_type, type2->array_type.element_type),
            false);
    if ((type1->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED ||
         type1->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC) &&
        (type2->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED ||
         type2->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC)) {
        REQUIRE(type1->array_type.const_length->value.integer == type2->array_type.const_length->value.integer, false);
    }
    return true;
}

const struct kefir_ast_type *composite_array_types(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                   const struct kefir_ast_type_traits *type_traits,
                                                   const struct kefir_ast_type *type1,
                                                   const struct kefir_ast_type *type2) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type_traits != NULL, NULL);
    REQUIRE(type1 != NULL, NULL);
    REQUIRE(type2 != NULL, NULL);
    REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, type2), NULL);
    if (type1->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC ||
        type1->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED) {
        return kefir_ast_type_array(
            mem, type_bundle,
            KEFIR_AST_TYPE_COMPOSITE(mem, type_bundle, type_traits, type1->array_type.element_type,
                                     type2->array_type.element_type),
            kefir_ast_constant_expression_integer(mem, type1->array_type.const_length->value.integer), NULL);
    } else if (type2->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC ||
               type2->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED) {
        return kefir_ast_type_array(
            mem, type_bundle,
            KEFIR_AST_TYPE_COMPOSITE(mem, type_bundle, type_traits, type1->array_type.element_type,
                                     type2->array_type.element_type),
            kefir_ast_constant_expression_integer(mem, type2->array_type.const_length->value.integer), NULL);
    } else if (type1->array_type.boundary == KEFIR_AST_ARRAY_VLA_STATIC ||
               type1->array_type.boundary == KEFIR_AST_ARRAY_VLA) {
        struct kefir_ast_node_base *vlen = KEFIR_AST_NODE_CLONE(mem, type1->array_type.vla_length);
        REQUIRE((vlen != NULL && type1->array_type.vla_length != NULL) ||
                    (vlen == NULL && type1->array_type.vla_length == NULL),
                NULL);
        const struct kefir_ast_type *composite_type = kefir_ast_type_vlen_array(
            mem, type_bundle,
            KEFIR_AST_TYPE_COMPOSITE(mem, type_bundle, type_traits, type1->array_type.element_type,
                                     type2->array_type.element_type),
            vlen, NULL);
        REQUIRE_ELSE(composite_type != NULL, {
            KEFIR_AST_NODE_FREE(mem, vlen);
            return NULL;
        });
        return composite_type;
    } else if (type2->array_type.boundary == KEFIR_AST_ARRAY_VLA_STATIC ||
               type2->array_type.boundary == KEFIR_AST_ARRAY_VLA) {
        struct kefir_ast_node_base *vlen = KEFIR_AST_NODE_CLONE(mem, type2->array_type.vla_length);
        REQUIRE((vlen != NULL && type2->array_type.vla_length != NULL) ||
                    (vlen == NULL && type2->array_type.vla_length == NULL),
                NULL);
        const struct kefir_ast_type *composite_type = kefir_ast_type_vlen_array(
            mem, type_bundle,
            KEFIR_AST_TYPE_COMPOSITE(mem, type_bundle, type_traits, type1->array_type.element_type,
                                     type2->array_type.element_type),
            vlen, NULL);
        REQUIRE_ELSE(composite_type != NULL, {
            KEFIR_AST_NODE_FREE(mem, vlen);
            return NULL;
        });
        return composite_type;
    } else {
        return kefir_ast_type_unbounded_array(
            mem, type_bundle,
            KEFIR_AST_TYPE_COMPOSITE(mem, type_bundle, type_traits, type1->array_type.element_type,
                                     type2->array_type.element_type),
            NULL);
    }
}

static kefir_result_t free_array(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    if (type->array_type.boundary == KEFIR_AST_ARRAY_VLA || type->array_type.boundary == KEFIR_AST_ARRAY_VLA_STATIC) {
        if (type->array_type.vla_length != NULL) {
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, type->array_type.vla_length));
        }
    } else if (type->array_type.boundary != KEFIR_AST_ARRAY_UNBOUNDED) {
        REQUIRE_OK(kefir_ast_constant_expression_free(mem, type->array_type.const_length));
    }
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

static struct kefir_ast_type *kefir_ast_type_array_impl(struct kefir_mem *mem,
                                                        struct kefir_ast_type_bundle *type_bundle,
                                                        const struct kefir_ast_type *element_type,
                                                        const struct kefir_ast_type_qualification *qualification,
                                                        struct kefir_ast_array_type **array_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(element_type != NULL, NULL);
    REQUIRE(array_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (type_bundle != NULL) {
        kefir_result_t res =
            kefir_list_insert_after(mem, &type_bundle->types, kefir_list_tail(&type_bundle->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_ARRAY;
    type->basic = false;
    type->ops.same = same_array_type;
    type->ops.compatible = compatbile_array_types;
    type->ops.composite = composite_array_types;
    type->ops.free = free_array;
    type->array_type.element_type = element_type;
    if (qualification != NULL) {
        type->array_type.qualifications = *qualification;
    } else {
        type->array_type.qualifications = (const struct kefir_ast_type_qualification){false};
    }
    *array_type = &type->array_type;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_unbounded_array(struct kefir_mem *mem,
                                                            struct kefir_ast_type_bundle *type_bundle,
                                                            const struct kefir_ast_type *element_type,
                                                            const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_bundle, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_UNBOUNDED;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_array(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                  const struct kefir_ast_type *element_type,
                                                  struct kefir_ast_constant_expression *const_length,
                                                  const struct kefir_ast_type_qualification *qualification) {
    REQUIRE(const_length != NULL, NULL);
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_bundle, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_BOUNDED;
    array_type->const_length = const_length;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_array_static(struct kefir_mem *mem,
                                                         struct kefir_ast_type_bundle *type_bundle,
                                                         const struct kefir_ast_type *element_type,
                                                         struct kefir_ast_constant_expression *const_length,
                                                         const struct kefir_ast_type_qualification *qualification) {
    REQUIRE(const_length != NULL, NULL);
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_bundle, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_BOUNDED_STATIC;
    array_type->const_length = const_length;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_vlen_array(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                       const struct kefir_ast_type *element_type,
                                                       struct kefir_ast_node_base *length,
                                                       const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_bundle, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_VLA;
    array_type->vla_length = length;
    return type;
}

const struct kefir_ast_type *kefir_ast_type_vlen_array_static(
    struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle, const struct kefir_ast_type *element_type,
    struct kefir_ast_node_base *length, const struct kefir_ast_type_qualification *qualification) {
    struct kefir_ast_array_type *array_type = NULL;
    struct kefir_ast_type *type = kefir_ast_type_array_impl(mem, type_bundle, element_type, qualification, &array_type);
    REQUIRE(type != NULL, NULL);
    array_type->boundary = KEFIR_AST_ARRAY_VLA_STATIC;
    array_type->vla_length = length;
    return type;
}

kefir_size_t kefir_ast_type_array_const_length(const struct kefir_ast_array_type *array) {
    REQUIRE(array != NULL, 0);
    REQUIRE(array->boundary == KEFIR_AST_ARRAY_BOUNDED || array->boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC, 0);
    return (kefir_size_t) array->const_length->value.integer;
}
