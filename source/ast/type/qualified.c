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

#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_qualified_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

static kefir_bool_t same_qualified_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_QUALIFIED && type2->tag == KEFIR_AST_TYPE_QUALIFIED, false);
    return type1->qualified_type.qualification.constant == type2->qualified_type.qualification.constant &&
           type1->qualified_type.qualification.restricted == type2->qualified_type.qualification.restricted &&
           type1->qualified_type.qualification.volatile_type == type2->qualified_type.qualification.volatile_type &&
           KEFIR_AST_TYPE_SAME(type1->qualified_type.type, type2->qualified_type.type);
}

static kefir_bool_t compatbile_qualified_types(const struct kefir_ast_type_traits *type_traits,
                                               const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    if (type1->tag == KEFIR_AST_TYPE_QUALIFIED && type2->tag != KEFIR_AST_TYPE_QUALIFIED) {
        return KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&type1->qualified_type.qualification) &&
               KEFIR_AST_TYPE_COMPATIBLE(type_traits, kefir_ast_unqualified_type(type1), type2);
    }
    REQUIRE(type1->tag == KEFIR_AST_TYPE_QUALIFIED && type2->tag == KEFIR_AST_TYPE_QUALIFIED, false);
    return type1->qualified_type.qualification.constant == type2->qualified_type.qualification.constant &&
           type1->qualified_type.qualification.restricted == type2->qualified_type.qualification.restricted &&
           type1->qualified_type.qualification.volatile_type == type2->qualified_type.qualification.volatile_type &&
           KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1->qualified_type.type, type2->qualified_type.type);
}

const struct kefir_ast_type *composite_qualified_types(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                       const struct kefir_ast_type_traits *type_traits,
                                                       const struct kefir_ast_type *type1,
                                                       const struct kefir_ast_type *type2) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type_traits != NULL, NULL);
    REQUIRE(type1 != NULL, NULL);
    REQUIRE(type2 != NULL, NULL);
    REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, type2), NULL);
    const struct kefir_ast_type *composite_unqualified = KEFIR_AST_TYPE_COMPOSITE(
        mem, type_bundle, type_traits, kefir_ast_unqualified_type(type1), kefir_ast_unqualified_type(type2));
    if (KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&type1->qualified_type.qualification)) {
        return composite_unqualified;
    } else {
        return kefir_ast_type_qualified(mem, type_bundle, composite_unqualified, type1->qualified_type.qualification);
    }
}

const struct kefir_ast_type *kefir_ast_type_qualified(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                      const struct kefir_ast_type *base_type,
                                                      struct kefir_ast_type_qualification qualification) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base_type != NULL, NULL);
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
    if (base_type->tag == KEFIR_AST_TYPE_QUALIFIED) {
        qualification.constant = qualification.constant || base_type->qualified_type.qualification.constant;
        qualification.restricted = qualification.restricted || base_type->qualified_type.qualification.restricted;
        qualification.volatile_type =
            qualification.volatile_type || base_type->qualified_type.qualification.volatile_type;
        base_type = base_type->qualified_type.type;
    }
    type->tag = KEFIR_AST_TYPE_QUALIFIED;
    type->basic = false;
    type->ops.same = same_qualified_type;
    type->ops.compatible = compatbile_qualified_types;
    type->ops.composite = composite_qualified_types;
    type->ops.free = free_qualified_type;
    type->qualified_type.qualification = qualification;
    type->qualified_type.type = base_type;
    return type;
}

const struct kefir_ast_type *kefir_ast_unqualified_type(const struct kefir_ast_type *type) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_QUALIFIED:
            return type->qualified_type.type;

        default:
            return type;
    }
    return NULL;
}

const struct kefir_ast_type *kefir_ast_zero_unqualified_type(const struct kefir_ast_type *type) {
    if (type->tag == KEFIR_AST_TYPE_QUALIFIED &&
        KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&type->qualified_type.qualification)) {
        return type->qualified_type.type;
    } else {
        return type;
    }
}

kefir_result_t kefir_ast_type_retrieve_qualifications(struct kefir_ast_type_qualification *qualifications,
                                                      const struct kefir_ast_type *type) {
    REQUIRE(qualifications != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type qualification pointer"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    if (type->tag == KEFIR_AST_TYPE_QUALIFIED) {
        *qualifications = type->qualified_type.qualification;
    } else {
        qualifications->constant = false;
        qualifications->restricted = false;
        qualifications->volatile_type = false;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_merge_qualifications(struct kefir_ast_type_qualification *dst,
                                                   const struct kefir_ast_type_qualification *src1,
                                                   const struct kefir_ast_type_qualification *src2) {
    REQUIRE(dst != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid destination AST type qualification pointer"));
    REQUIRE(src1 != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid first source AST type qualification pointer"));
    REQUIRE(src2 != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid second source AST type qualification pointer"));
    dst->constant = src1->constant || src2->constant;
    dst->restricted = src1->restricted || src2->restricted;
    dst->volatile_type = src1->volatile_type || src2->volatile_type;
    return KEFIR_OK;
}
