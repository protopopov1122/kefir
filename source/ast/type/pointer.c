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

#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t same_pointer_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER, false);
    return KEFIR_AST_TYPE_SAME(type1->referenced_type, type2->referenced_type);
}

static kefir_bool_t compatible_pointer_types(const struct kefir_ast_type_traits *type_traits,
                                             const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER && type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER, false);
    return KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1->referenced_type, type2->referenced_type);
}

const struct kefir_ast_type *composite_pointer_types(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                     const struct kefir_ast_type_traits *type_traits,
                                                     const struct kefir_ast_type *type1,
                                                     const struct kefir_ast_type *type2) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type_traits != NULL, NULL);
    REQUIRE(type1 != NULL, NULL);
    REQUIRE(type2 != NULL, NULL);
    REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, type2), NULL);
    return kefir_ast_type_pointer(
        mem, type_bundle,
        KEFIR_AST_TYPE_COMPOSITE(mem, type_bundle, type_traits, kefir_ast_pointer_referenced_type(type1),
                                 kefir_ast_pointer_referenced_type(type2)));
}

static kefir_result_t free_pointer_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_pointer(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle,
                                                    const struct kefir_ast_type *base_type) {
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
    type->tag = KEFIR_AST_TYPE_SCALAR_POINTER;
    type->basic = false;
    type->ops.same = same_pointer_type;
    type->ops.compatible = compatible_pointer_types;
    type->ops.composite = composite_pointer_types;
    type->ops.free = free_pointer_type;
    type->referenced_type = base_type;
    return type;
}

const struct kefir_ast_type *kefir_ast_pointer_referenced_type(const struct kefir_ast_type *pointer_type) {
    REQUIRE(pointer_type != NULL, NULL);
    return pointer_type->referenced_type;
}
