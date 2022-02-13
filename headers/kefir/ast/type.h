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

#ifndef KEFIR_AST_TYPE_H_
#define KEFIR_AST_TYPE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/base.h"
#include "kefir/ast/type/base.h"
#include "kefir/ast/type/basic.h"
#include "kefir/ast/type/struct.h"
#include "kefir/ast/type/enum.h"
#include "kefir/ast/type/qualified.h"
#include "kefir/ast/type/array.h"
#include "kefir/ast/type/pointer.h"
#include "kefir/ast/type/function.h"
#include "kefir/core/data_model.h"

typedef kefir_uint64_t kefir_ast_type_hash_t;

typedef struct kefir_ast_type_ops {
    kefir_bool_t (*same)(const struct kefir_ast_type *, const struct kefir_ast_type *);
    kefir_bool_t (*compatible)(const struct kefir_ast_type_traits *, const struct kefir_ast_type *,
                               const struct kefir_ast_type *);
    const struct kefir_ast_type *(*composite)(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                              const struct kefir_ast_type_traits *, const struct kefir_ast_type *,
                                              const struct kefir_ast_type *);
    kefir_result_t (*free)(struct kefir_mem *, const struct kefir_ast_type *);
} kefir_ast_type_ops_t;

typedef struct kefir_ast_type {
    kefir_ast_type_tag_t tag;
    kefir_bool_t basic;
    struct kefir_ast_type_ops ops;
    union {
        struct kefir_ast_basic_type_properties basic_type;
        const struct kefir_ast_type *referenced_type;
        struct kefir_ast_enum_type enumeration_type;
        struct kefir_ast_struct_type structure_type;
        struct kefir_ast_array_type array_type;
        struct kefir_ast_function_type function_type;
        struct kefir_ast_qualified_type qualified_type;
    };
} kefir_ast_type_t;

typedef struct kefir_ast_type_traits {
    kefir_result_t (*integral_type_fits)(const struct kefir_ast_type_traits *, const struct kefir_ast_type *,
                                         const struct kefir_ast_type *, kefir_bool_t *);
    const struct kefir_ast_type *(*bitfield_promotion)(const struct kefir_ast_type_traits *,
                                                       const struct kefir_ast_type *, kefir_size_t);
    const struct kefir_ast_type *underlying_enumeration_type;
    const struct kefir_ast_type *ptrdiff_type;
    const struct kefir_ast_type *wide_char_type;
    const struct kefir_ast_type *unicode16_char_type;
    const struct kefir_ast_type *unicode32_char_type;
    const struct kefir_ast_type *incomplete_type_substitute;
    kefir_bool_t character_type_signedness;

    kefir_uptr_t payload;
} kefir_ast_type_traits_t;

typedef struct kefir_ast_type_bundle {
    struct kefir_symbol_table *symbols;
    struct kefir_list types;
} kefir_ast_type_bundle_t;

kefir_result_t kefir_ast_type_traits_init(const struct kefir_data_model_descriptor *, struct kefir_ast_type_traits *);
kefir_bool_t kefir_ast_type_is_complete(const struct kefir_ast_type *);
kefir_result_t kefir_ast_type_list_variable_modificators(const struct kefir_ast_type *,
                                                         kefir_result_t (*)(const struct kefir_ast_node_base *, void *),
                                                         void *);
const struct kefir_ast_node_base *kefir_ast_type_get_top_variable_modificator(const struct kefir_ast_type *);
kefir_bool_t kefir_ast_type_is_variably_modified(const struct kefir_ast_type *);

kefir_result_t kefir_ast_type_bundle_init(struct kefir_ast_type_bundle *, struct kefir_symbol_table *);
kefir_result_t kefir_ast_type_bundle_free(struct kefir_mem *, struct kefir_ast_type_bundle *);

kefir_ast_function_specifier_t kefir_ast_context_merge_function_specifiers(kefir_ast_function_specifier_t,
                                                                           kefir_ast_function_specifier_t);

#define KEFIR_AST_TYPE_SAME(type1, type2) ((type1)->ops.same((type1), (type2)))
#define KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, type2)                   \
    (KEFIR_AST_TYPE_SAME((type1), (kefir_ast_zero_unqualified_type(type2))) || \
     (type1)->ops.compatible((type_traits), (type1), (kefir_ast_zero_unqualified_type(type2))))
#define KEFIR_AST_TYPE_COMPOSITE(mem, type_bundle, type_traits, type1, type2) \
    ((type1)->ops.composite((mem), (type_bundle), (type_traits), (type1), (type2)))
#define KEFIR_AST_TYPE_FREE(mem, type) ((type)->ops.free((mem), (type)))
#define KEFIR_AST_TYPE_IS_INCOMPLETE_IMPL(type)                                                           \
    ((type)->tag == KEFIR_AST_TYPE_VOID ||                                                                \
     ((type)->tag == KEFIR_AST_TYPE_ARRAY && (type)->array_type.boundary == KEFIR_AST_ARRAY_UNBOUNDED) || \
     (((type)->tag == KEFIR_AST_TYPE_STRUCTURE || (type->tag) == KEFIR_AST_TYPE_UNION) &&                 \
      !(type)->structure_type.complete))
#define KEFIR_AST_TYPE_IS_INCOMPLETE(type) KEFIR_AST_TYPE_IS_INCOMPLETE_IMPL(kefir_ast_unqualified_type((type)))
#define KEFIR_AST_TYPE_IS_VL_ARRAY(type)                                                           \
    ((type)->tag == KEFIR_AST_TYPE_ARRAY && ((type)->array_type.boundary == KEFIR_AST_ARRAY_VLA || \
                                             (type)->array_type.boundary == KEFIR_AST_ARRAY_VLA_STATIC))
#define KEFIR_AST_TYPE_HASH(type) ((kefir_ast_type_hash_t) type)

extern const struct kefir_ast_bitfield_properties KEFIR_AST_BITFIELD_PROPERTIES_NONE;

#endif
