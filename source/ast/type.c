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

#include <string.h>
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_size_t default_integral_type_fit_rank(const struct kefir_ast_type *type) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_SCALAR_BOOL:
        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            return 1;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            return 2;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
            return 4;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            return 8;

        default:
            return 0;
    }
}

static kefir_result_t default_integral_type_fits(const struct kefir_ast_type_traits *type_traits,
                                                 const struct kefir_ast_type *source, const struct kefir_ast_type *dest,
                                                 kefir_bool_t *result) {
    UNUSED(type_traits);
    REQUIRE(source != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid source AST type"));
    REQUIRE(dest != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid destination AST type"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid result pointer"));
    REQUIRE((KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(source) || source->tag == KEFIR_AST_TYPE_SCALAR_BOOL) &&
                (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(dest) || dest->tag == KEFIR_AST_TYPE_SCALAR_BOOL),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected both source and destination to be basic types"));
    kefir_size_t source_fit = default_integral_type_fit_rank(source);
    kefir_size_t dest_fit = default_integral_type_fit_rank(dest);
    REQUIRE(source_fit != 0 && dest_fit != 0, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected integral type"));

    kefir_bool_t src_sign, dst_sign;
    REQUIRE_OK(kefir_ast_type_is_signed(type_traits, source, &src_sign));
    REQUIRE_OK(kefir_ast_type_is_signed(type_traits, dest, &dst_sign));

    if (src_sign == dst_sign || (src_sign && !dst_sign)) {
        *result = source_fit <= dest_fit;
    } else if (!src_sign && dst_sign) {
        *result = source_fit < dest_fit;
    } else {
        *result = false;
    }
    return KEFIR_OK;
}

const struct kefir_ast_type_traits *kefir_ast_default_type_traits() {
    static struct kefir_ast_type_traits DEFAULT_TYPE_TRAITS = {.integral_type_fits = default_integral_type_fits,
                                                               .underlying_enumeration_type = NULL,
                                                               .ptrdiff_type = NULL,
                                                               .character_type_signedness = true,
                                                               .payload = NULL};
    DEFAULT_TYPE_TRAITS.underlying_enumeration_type = kefir_ast_type_signed_int();
    DEFAULT_TYPE_TRAITS.ptrdiff_type = kefir_ast_type_signed_long();
    return &DEFAULT_TYPE_TRAITS;
}

kefir_bool_t kefir_ast_type_is_complete(const struct kefir_ast_type *type) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
            return type->structure_type.complete;

        case KEFIR_AST_TYPE_ENUMERATION:
            return type->enumeration_type.complete;

        default:
            return true;
    }
}

static kefir_result_t free_type_bundle(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                       void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    REQUIRE_OK(KEFIR_AST_TYPE_FREE(mem, (struct kefir_ast_type *) entry->value));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_bundle_init(struct kefir_ast_type_bundle *type_bundle,
                                          struct kefir_symbol_table *symbols) {
    REQUIRE(type_bundle != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type type_bundlesitory"));
    type_bundle->symbols = symbols;
    REQUIRE_OK(kefir_list_init(&type_bundle->types));
    REQUIRE_OK(kefir_list_on_remove(&type_bundle->types, free_type_bundle, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_bundle_free(struct kefir_mem *mem, struct kefir_ast_type_bundle *type_bundle) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_bundle != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type type_bundlesitory"));
    REQUIRE_OK(kefir_list_free(mem, &type_bundle->types));
    return KEFIR_OK;
}

kefir_ast_function_specifier_t kefir_ast_context_merge_function_specifiers(kefir_ast_function_specifier_t s1,
                                                                           kefir_ast_function_specifier_t s2) {
    _Static_assert(KEFIR_AST_FUNCTION_SPECIFIER_NONE < 4,
                   "AST function specifier is expected to fit into conversion matrix");
    _Static_assert(KEFIR_AST_FUNCTION_SPECIFIER_INLINE < 4,
                   "AST function specifier is expected to fit into conversion matrix");
    _Static_assert(KEFIR_AST_FUNCTION_SPECIFIER_NORETURN < 4,
                   "AST function specifier is expected to fit into conversion matrix");
    _Static_assert(KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN < 4,
                   "AST function specifier is expected to fit into conversion matrix");
    kefir_ast_function_specifier_t MATRIX[4][4] = {
        [KEFIR_AST_FUNCTION_SPECIFIER_NONE] =
            {[KEFIR_AST_FUNCTION_SPECIFIER_NONE] = KEFIR_AST_FUNCTION_SPECIFIER_NONE,
             [KEFIR_AST_FUNCTION_SPECIFIER_INLINE] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
             [KEFIR_AST_FUNCTION_SPECIFIER_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_NORETURN,
             [KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN},
        [KEFIR_AST_FUNCTION_SPECIFIER_INLINE] =
            {[KEFIR_AST_FUNCTION_SPECIFIER_NONE] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
             [KEFIR_AST_FUNCTION_SPECIFIER_INLINE] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE,
             [KEFIR_AST_FUNCTION_SPECIFIER_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN,
             [KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN},
        [KEFIR_AST_FUNCTION_SPECIFIER_NORETURN] =
            {[KEFIR_AST_FUNCTION_SPECIFIER_NONE] = KEFIR_AST_FUNCTION_SPECIFIER_NORETURN,
             [KEFIR_AST_FUNCTION_SPECIFIER_INLINE] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN,
             [KEFIR_AST_FUNCTION_SPECIFIER_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_NORETURN,
             [KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN},
        [KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN] = {
            [KEFIR_AST_FUNCTION_SPECIFIER_NONE] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN,
            [KEFIR_AST_FUNCTION_SPECIFIER_INLINE] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN,
            [KEFIR_AST_FUNCTION_SPECIFIER_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN,
            [KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN] = KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN}};
    return MATRIX[s1][s2];
}
