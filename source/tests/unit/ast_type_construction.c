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

#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"
#include "kefir/ast/type.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

static kefir_bool_t is_signed(const struct kefir_ast_type *type) {
    kefir_bool_t signedness;
    ASSERT_OK(kefir_ast_type_is_signed(kefir_ast_default_type_traits(), type, &signedness));
    return signedness;
}

static kefir_bool_t is_unsigned(const struct kefir_ast_type *type) {
    return !is_signed(type);
}

DEFINE_CASE(ast_type_construction1, "AST Types - basic types") {
    const struct kefir_ast_type *BASIC_TYPES[] = {kefir_ast_type_void(),
                                                  kefir_ast_type_bool(),
                                                  kefir_ast_type_char(),
                                                  kefir_ast_type_unsigned_char(),
                                                  kefir_ast_type_signed_char(),
                                                  kefir_ast_type_unsigned_short(),
                                                  kefir_ast_type_signed_short(),
                                                  kefir_ast_type_unsigned_int(),
                                                  kefir_ast_type_signed_int(),
                                                  kefir_ast_type_unsigned_long(),
                                                  kefir_ast_type_signed_long(),
                                                  kefir_ast_type_unsigned_long_long(),
                                                  kefir_ast_type_signed_long_long(),
                                                  kefir_ast_type_float(),
                                                  kefir_ast_type_double()};
    const kefir_size_t BASIC_TYPES_LENGTH = sizeof(BASIC_TYPES) / sizeof(BASIC_TYPES[0]);
    for (kefir_size_t i = 0; i < BASIC_TYPES_LENGTH; i++) {
        ASSERT(BASIC_TYPES[i] != NULL);
        for (kefir_size_t j = 0; j < BASIC_TYPES_LENGTH; j++) {
            ASSERT(BASIC_TYPES[j] != NULL);
            if (i != j) {
                ASSERT(!KEFIR_AST_TYPE_SAME(BASIC_TYPES[i], BASIC_TYPES[j]));
            } else {
                ASSERT(KEFIR_AST_TYPE_SAME(BASIC_TYPES[i], BASIC_TYPES[j]));
            }
        }
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_unqualified_type(BASIC_TYPES[i]), BASIC_TYPES[i]));
    }

    ASSERT(KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_char()));
    ASSERT(KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_unsigned_char()));
    ASSERT(KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_signed_char()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_unsigned_short()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_signed_short()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_unsigned_int()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_signed_int()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_unsigned_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_signed_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_unsigned_long_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_signed_long_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_void()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_bool()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_float()));
    ASSERT(!KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_type_double()));

    ASSERT(is_signed(kefir_ast_type_char()) == kefir_ast_default_type_traits()->character_type_signedness);
    ASSERT(!is_signed(kefir_ast_type_unsigned_char()));
    ASSERT(is_signed(kefir_ast_type_signed_char()));
    ASSERT(!is_signed(kefir_ast_type_unsigned_short()));
    ASSERT(is_signed(kefir_ast_type_signed_short()));
    ASSERT(!is_signed(kefir_ast_type_unsigned_int()));
    ASSERT(is_signed(kefir_ast_type_signed_int()));
    ASSERT(!is_signed(kefir_ast_type_unsigned_long()));
    ASSERT(is_signed(kefir_ast_type_signed_long()));
    ASSERT(!is_signed(kefir_ast_type_unsigned_long_long()));
    ASSERT(is_signed(kefir_ast_type_signed_long_long()));
    ASSERT(!is_signed(kefir_ast_type_bool()));

    ASSERT(is_unsigned(kefir_ast_type_char()) == !kefir_ast_default_type_traits()->character_type_signedness);
    ASSERT(is_unsigned(kefir_ast_type_unsigned_char()));
    ASSERT(!is_unsigned(kefir_ast_type_signed_char()));
    ASSERT(is_unsigned(kefir_ast_type_unsigned_short()));
    ASSERT(!is_unsigned(kefir_ast_type_signed_short()));
    ASSERT(is_unsigned(kefir_ast_type_unsigned_int()));
    ASSERT(!is_unsigned(kefir_ast_type_signed_int()));
    ASSERT(is_unsigned(kefir_ast_type_unsigned_long()));
    ASSERT(!is_unsigned(kefir_ast_type_signed_long()));
    ASSERT(is_unsigned(kefir_ast_type_unsigned_long_long()));
    ASSERT(!is_unsigned(kefir_ast_type_signed_long_long()));
    ASSERT(is_unsigned(kefir_ast_type_bool()));

    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_char()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_unsigned_char()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_signed_char()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_unsigned_short()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_signed_short()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_unsigned_int()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_signed_int()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_unsigned_long()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_signed_long()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_unsigned_long_long()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_signed_long_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_void()));
    ASSERT(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_bool()));
    ASSERT(!KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_float()));
    ASSERT(!KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(kefir_ast_type_double()));

    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_char()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_unsigned_char()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_signed_char()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_unsigned_short()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_signed_short()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_unsigned_int()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_signed_int()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_unsigned_long()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_signed_long()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_unsigned_long_long()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_signed_long_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_void()));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_bool()));
    ASSERT(!KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_float()));
    ASSERT(!KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(kefir_ast_type_double()));

    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_char()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_unsigned_char()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_signed_char()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_unsigned_short()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_signed_short()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_unsigned_int()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_signed_int()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_unsigned_long()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_signed_long()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_unsigned_long_long()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_signed_long_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_void()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_bool()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_float()));
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(kefir_ast_type_double()));

    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_char()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_unsigned_char()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_signed_char()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_unsigned_short()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_signed_short()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_unsigned_int()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_signed_int()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_unsigned_long()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_signed_long()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_unsigned_long_long()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_signed_long_long()));
    ASSERT(!KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_void()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_bool()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_float()));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(kefir_ast_type_double()));
}
END_CASE

DEFINE_CASE(ast_type_construction2, "AST Types - pointer type") {
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *BASE_TYPES[] = {
        kefir_ast_type_void(),
        kefir_ast_type_bool(),
        kefir_ast_type_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_short(),
        kefir_ast_type_signed_short(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double(),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_bool()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_char()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_short()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_short()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_int()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_int()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_double()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_bool())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_char())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_short())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_short())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_int())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_int())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_double()))};
    const kefir_size_t BASE_TYPES_LENGTH = sizeof(BASE_TYPES) / sizeof(BASE_TYPES[0]);
    for (kefir_size_t i = 0; i < BASE_TYPES_LENGTH; i++) {
        ASSERT(BASE_TYPES[i] != NULL);
        const struct kefir_ast_type *pointer_type = kefir_ast_type_pointer(&kft_mem, &type_bundle, BASE_TYPES[i]);
        ASSERT(pointer_type != NULL);
        ASSERT(pointer_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER);
        ASSERT(!KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(pointer_type));
        ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(pointer_type));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_pointer_referenced_type(pointer_type), BASE_TYPES[i]));
    }

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_type_construction3, "AST Types - qualified type") {
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *BASE_TYPES[] = {
        kefir_ast_type_void(),
        kefir_ast_type_bool(),
        kefir_ast_type_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_short(),
        kefir_ast_type_signed_short(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double(),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_bool()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_char()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_short()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_short()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_int()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_int()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long_long()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_double()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_bool())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_char())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_short())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_short())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_int())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_int())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_long_long())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float())),
        kefir_ast_type_pointer(&kft_mem, &type_bundle,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_double()))};
    const kefir_size_t BASE_TYPES_LENGTH = sizeof(BASE_TYPES) / sizeof(BASE_TYPES[0]);
    for (kefir_size_t i = 0; i < BASE_TYPES_LENGTH; i++) {
        ASSERT(BASE_TYPES[i] != NULL);
        const struct kefir_ast_type *qualified_type = kefir_ast_type_qualified(
            &kft_mem, &type_bundle, BASE_TYPES[i],
            (struct kefir_ast_type_qualification){.constant = false, .restricted = false, .volatile_type = false});
        ASSERT(qualified_type != NULL);
        ASSERT(qualified_type->tag == KEFIR_AST_TYPE_QUALIFIED);
        ASSERT(!KEFIR_AST_TYPE_IS_SCALAR_TYPE(qualified_type));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_unqualified_type(qualified_type), BASE_TYPES[i]));
        ASSERT(!qualified_type->qualified_type.qualification.constant);
        ASSERT(!qualified_type->qualified_type.qualification.restricted);
        ASSERT(!qualified_type->qualified_type.qualification.volatile_type);

        const struct kefir_ast_type *qualified_type2 = kefir_ast_type_qualified(
            &kft_mem, &type_bundle, qualified_type,
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false});
        ASSERT(qualified_type2 != NULL);
        ASSERT(qualified_type2->tag == KEFIR_AST_TYPE_QUALIFIED);
        ASSERT(!KEFIR_AST_TYPE_IS_SCALAR_TYPE(qualified_type2));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_unqualified_type(qualified_type2), BASE_TYPES[i]));
        ASSERT(qualified_type2->qualified_type.qualification.constant);
        ASSERT(!qualified_type2->qualified_type.qualification.restricted);
        ASSERT(!qualified_type2->qualified_type.qualification.volatile_type);

        const struct kefir_ast_type *qualified_type3 = kefir_ast_type_qualified(
            &kft_mem, &type_bundle, qualified_type2,
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = false});
        ASSERT(qualified_type3 != NULL);
        ASSERT(qualified_type3->tag == KEFIR_AST_TYPE_QUALIFIED);
        ASSERT(!KEFIR_AST_TYPE_IS_SCALAR_TYPE(qualified_type3));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_unqualified_type(qualified_type3), BASE_TYPES[i]));
        ASSERT(qualified_type3->qualified_type.qualification.constant);
        ASSERT(qualified_type3->qualified_type.qualification.restricted);
        ASSERT(!qualified_type3->qualified_type.qualification.volatile_type);

        const struct kefir_ast_type *qualified_type4 = kefir_ast_type_qualified(
            &kft_mem, &type_bundle, qualified_type3,
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = true});
        ASSERT(qualified_type4 != NULL);
        ASSERT(qualified_type4->tag == KEFIR_AST_TYPE_QUALIFIED);
        ASSERT(!KEFIR_AST_TYPE_IS_SCALAR_TYPE(qualified_type4));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_unqualified_type(qualified_type4), BASE_TYPES[i]));
        ASSERT(qualified_type4->qualified_type.qualification.constant);
        ASSERT(qualified_type4->qualified_type.qualification.restricted);
        ASSERT(qualified_type4->qualified_type.qualification.volatile_type);
    }

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

#define ASSERT_ENUM_CONSTANT(_enum, _id, _value)                      \
    do {                                                              \
        const struct kefir_ast_constant_expression *_cnst;            \
        ASSERT_OK(kefir_ast_enumeration_get((_enum), (_id), &_cnst)); \
        ASSERT(_cnst->value.integer == (_value));                     \
    } while (0)

#define ASSERT_NO_ENUM_CONSTANT(_enum, _id)                                           \
    do {                                                                              \
        const struct kefir_ast_constant_expression *_cnst;                            \
        ASSERT(kefir_ast_enumeration_get((_enum), (_id), &_cnst) == KEFIR_NOT_FOUND); \
    } while (0)

DEFINE_CASE(ast_type_construction4, "AST Types - enum type") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_enum_type *enum1_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(
        &kft_mem, context->type_bundle, "enum1", type_traits->underlying_enumeration_type, &enum1_type);
    ASSERT(type1 != NULL);
    ASSERT(type1->tag == KEFIR_AST_TYPE_ENUMERATION);
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type1));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type1));
    ASSERT(KEFIR_AST_TYPE_SAME(type1, type1));
    ASSERT(enum1_type->complete);
    ASSERT(strcmp(enum1_type->identifier, "enum1") == 0);
    ASSERT(KEFIR_AST_TYPE_SAME(type_traits->underlying_enumeration_type,
                               kefir_ast_enumeration_underlying_type(enum1_type)));

    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum1_type, "c1",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 10)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum1_type, "c2"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum1_type, "c3"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum1_type, "c4",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum1_type, "c5"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, context->symbols, enum1_type, "c10",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 0)));
    ASSERT(kefir_list_length(&enum1_type->enumerators) == 6);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));
    ASSERT_ENUM_CONSTANT(enum1_type, "c1", 10);
    ASSERT_ENUM_CONSTANT(enum1_type, "c2", 11);
    ASSERT_ENUM_CONSTANT(enum1_type, "c3", 12);
    ASSERT_ENUM_CONSTANT(enum1_type, "c4", 1);
    ASSERT_ENUM_CONSTANT(enum1_type, "c5", 2);
    ASSERT_ENUM_CONSTANT(enum1_type, "c10", 0);
    ASSERT_NO_ENUM_CONSTANT(enum1_type, "c0");
    ASSERT_NO_ENUM_CONSTANT(enum1_type, "c6");

    const struct kefir_ast_type *type2 = kefir_ast_type_incomplete_enumeration(
        &kft_mem, context->type_bundle, "enum2", type_traits->underlying_enumeration_type);
    ASSERT(type2 != NULL);
    ASSERT(type2->tag == KEFIR_AST_TYPE_ENUMERATION);
    ASSERT(KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(type2));
    ASSERT(KEFIR_AST_TYPE_IS_SCALAR_TYPE(type2));
    ASSERT(KEFIR_AST_TYPE_SAME(type2, type2));
    ASSERT(!KEFIR_AST_TYPE_SAME(type2, type1));
    ASSERT(!KEFIR_AST_TYPE_SAME(type1, type2));
    ASSERT(!type2->enumeration_type.complete);
    ASSERT(strcmp(type2->enumeration_type.identifier, "enum2") == 0);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

#undef ASSERT_ENUM_CONSTANT
#undef ASSERT_NO_ENUM_CONSTANT

DEFINE_CASE(ast_type_construction5, "AST Types - array type") {
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 = kefir_ast_type_unbounded_array(
        &kft_mem, &type_bundle, kefir_ast_type_signed_int(),
        &(const struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = true});

    ASSERT(type1 != NULL);
    ASSERT(type1->tag == KEFIR_AST_TYPE_ARRAY);
    ASSERT(type1->array_type.boundary == KEFIR_AST_ARRAY_UNBOUNDED);
    ASSERT(KEFIR_AST_TYPE_SAME(type1, type1));
    ASSERT(KEFIR_AST_TYPE_SAME(type1->array_type.element_type, kefir_ast_type_signed_int()));
    ASSERT(type1->array_type.qualifications.constant);
    ASSERT(!type1->array_type.qualifications.restricted);
    ASSERT(type1->array_type.qualifications.volatile_type);

    const struct kefir_ast_type *type2 = kefir_ast_type_array(
        &kft_mem, &type_bundle, kefir_ast_type_float(), kefir_ast_constant_expression_integer(&kft_mem, 10),
        &(const struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = false});
    ASSERT(type2 != NULL);
    ASSERT(type2->tag == KEFIR_AST_TYPE_ARRAY);
    ASSERT(type2->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED);
    ASSERT(KEFIR_AST_TYPE_SAME(type2, type2));
    ASSERT(KEFIR_AST_TYPE_SAME(type2->array_type.element_type, kefir_ast_type_float()));
    ASSERT(kefir_ast_type_array_const_length(&type2->array_type) == 10);
    ASSERT(!type2->array_type.qualifications.constant);
    ASSERT(type2->array_type.qualifications.restricted);
    ASSERT(!type2->array_type.qualifications.volatile_type);

    const struct kefir_ast_type *type3 = kefir_ast_type_array_static(
        &kft_mem, &type_bundle, kefir_ast_type_double(), kefir_ast_constant_expression_integer(&kft_mem, 15),
        &(const struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = true});
    ASSERT(type3 != NULL);
    ASSERT(type3->tag == KEFIR_AST_TYPE_ARRAY);
    ASSERT(type3->array_type.boundary == KEFIR_AST_ARRAY_BOUNDED_STATIC);
    ASSERT(KEFIR_AST_TYPE_SAME(type3, type3));
    ASSERT(KEFIR_AST_TYPE_SAME(type3->array_type.element_type, kefir_ast_type_double()));
    ASSERT(kefir_ast_type_array_const_length(&type3->array_type) == 15);
    ASSERT(!type3->array_type.qualifications.constant);
    ASSERT(type3->array_type.qualifications.restricted);
    ASSERT(type3->array_type.qualifications.volatile_type);

    const struct kefir_ast_type *type4 = kefir_ast_type_vlen_array(
        &kft_mem, &type_bundle, kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char()), NULL,
        &(const struct kefir_ast_type_qualification){.constant = false, .restricted = false, .volatile_type = false});
    ASSERT(type4 != NULL);
    ASSERT(type4->tag == KEFIR_AST_TYPE_ARRAY);
    ASSERT(type4->array_type.boundary == KEFIR_AST_ARRAY_VLA);
    ASSERT(KEFIR_AST_TYPE_SAME(type4, type4));
    ASSERT(KEFIR_AST_TYPE_SAME(type4->array_type.element_type,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char())));
    ASSERT(type4->array_type.vla_length == NULL);
    ASSERT(!type4->array_type.qualifications.constant);
    ASSERT(!type4->array_type.qualifications.restricted);
    ASSERT(!type4->array_type.qualifications.volatile_type);

    struct kefir_ast_constant *type5_len = kefir_ast_new_constant_int(&kft_mem, 5);
    const struct kefir_ast_type *type5 = kefir_ast_type_vlen_array_static(
        &kft_mem, &type_bundle, kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_char()),
        KEFIR_AST_NODE_BASE(type5_len),
        &(const struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false});
    ASSERT(type5 != NULL);
    ASSERT(type5->tag == KEFIR_AST_TYPE_ARRAY);
    ASSERT(type5->array_type.boundary == KEFIR_AST_ARRAY_VLA_STATIC);
    ASSERT(KEFIR_AST_TYPE_SAME(type5, type5));
    ASSERT(KEFIR_AST_TYPE_SAME(type5->array_type.element_type,
                               kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_char())));
    ASSERT(type5->array_type.vla_length == KEFIR_AST_NODE_BASE(type5_len));
    ASSERT(type5->array_type.qualifications.constant);
    ASSERT(!type5->array_type.qualifications.restricted);
    ASSERT(!type5->array_type.qualifications.volatile_type);

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

#define ASSERT_STRUCT_FIELD(_struct, _id, _type)                               \
    do {                                                                       \
        const struct kefir_ast_struct_field *_field = NULL;                    \
        ASSERT_OK(kefir_ast_struct_type_get_field((_struct), (_id), &_field)); \
        ASSERT(KEFIR_AST_TYPE_SAME(_field->type, (_type)));                    \
        ASSERT(!_field->bitfield);                                             \
    } while (0)

#define ASSERT_STRUCT_BITFIELD(_struct, _id, _type, _width)                    \
    do {                                                                       \
        const struct kefir_ast_struct_field *_field = NULL;                    \
        ASSERT_OK(kefir_ast_struct_type_get_field((_struct), (_id), &_field)); \
        ASSERT(KEFIR_AST_TYPE_SAME(_field->type, (_type)));                    \
        ASSERT(_field->bitfield);                                              \
        ASSERT(_field->bitwidth->value.integer == (_width));                   \
    } while (0)

DEFINE_CASE(ast_type_construction6, "AST Types - struct type") {
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 = kefir_ast_type_incomplete_structure(&kft_mem, &type_bundle, "struct1");
    ASSERT(type1 != NULL);
    ASSERT(type1->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(!type1->structure_type.complete);
    ASSERT(type1->structure_type.identifier != NULL && strcmp(type1->structure_type.identifier, "struct1") == 0);
    ASSERT(KEFIR_AST_TYPE_SAME(type1, type1));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, &type_bundle, "struct2", &struct_type2);
    ASSERT(type2 != NULL);
    ASSERT(struct_type2 != NULL);
    ASSERT(type2->tag == KEFIR_AST_TYPE_STRUCTURE);
    ASSERT(type2->structure_type.complete);
    ASSERT(type2->structure_type.identifier != NULL && strcmp(type2->structure_type.identifier, "struct2") == 0);

    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type2, "field0", kefir_ast_type_unsigned_short(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type2, "field1",
                                          kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()),
                                          NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, &symbols, struct_type2, "field10", kefir_ast_type_unsigned_int(),
                                             NULL, kefir_ast_constant_expression_integer(&kft_mem, 5)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, &symbols, struct_type2, "field11", kefir_ast_type_unsigned_int(),
                                             NULL, kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, &symbols, struct_type2, "field20",
                                             kefir_ast_type_unsigned_long(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 16)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type2, "field21", kefir_ast_type_bool(), NULL));

    ASSERT(KEFIR_AST_TYPE_SAME(type2, type2));
    ASSERT(!KEFIR_AST_TYPE_SAME(type1, type2));
    ASSERT(!KEFIR_AST_TYPE_SAME(type2, type1));
    ASSERT(kefir_list_length(&struct_type2->fields) == 6);
    ASSERT_STRUCT_FIELD(struct_type2, "field0", kefir_ast_type_unsigned_short());
    ASSERT_STRUCT_FIELD(struct_type2, "field1", kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()));
    ASSERT_STRUCT_BITFIELD(struct_type2, "field10", kefir_ast_type_unsigned_int(), 5);
    ASSERT_STRUCT_BITFIELD(struct_type2, "field11", kefir_ast_type_unsigned_int(), 3);
    ASSERT_STRUCT_BITFIELD(struct_type2, "field20", kefir_ast_type_unsigned_long(), 16);
    ASSERT_STRUCT_FIELD(struct_type2, "field21", kefir_ast_type_bool());

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_type_construction7, "AST Types - union type") {
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 = kefir_ast_type_incomplete_union(&kft_mem, &type_bundle, "union1");
    ASSERT(type1 != NULL);
    ASSERT(type1->tag == KEFIR_AST_TYPE_UNION);
    ASSERT(!type1->structure_type.complete);
    ASSERT(type1->structure_type.identifier != NULL && strcmp(type1->structure_type.identifier, "union1") == 0);
    ASSERT(KEFIR_AST_TYPE_SAME(type1, type1));

    struct kefir_ast_struct_type *union_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_union(&kft_mem, &type_bundle, "union2", &union_type2);
    ASSERT(type2 != NULL);
    ASSERT(union_type2 != NULL);
    ASSERT(type2->tag == KEFIR_AST_TYPE_UNION);
    ASSERT(type2->structure_type.complete);
    ASSERT(type2->structure_type.identifier != NULL && strcmp(type2->structure_type.identifier, "union2") == 0);

    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type2, "field0", kefir_ast_type_unsigned_short(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type2, "field1",
                                          kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()),
                                          NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, &symbols, union_type2, "field10", kefir_ast_type_unsigned_int(),
                                             NULL, kefir_ast_constant_expression_integer(&kft_mem, 5)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, &symbols, union_type2, "field11", kefir_ast_type_unsigned_int(),
                                             NULL, kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, &symbols, union_type2, "field20", kefir_ast_type_unsigned_long(),
                                             NULL, kefir_ast_constant_expression_integer(&kft_mem, 16)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type2, "field21", kefir_ast_type_bool(), NULL));

    ASSERT(KEFIR_AST_TYPE_SAME(type2, type2));
    ASSERT(!KEFIR_AST_TYPE_SAME(type1, type2));
    ASSERT(!KEFIR_AST_TYPE_SAME(type2, type1));
    ASSERT(kefir_list_length(&union_type2->fields) == 6);
    ASSERT_STRUCT_FIELD(union_type2, "field0", kefir_ast_type_unsigned_short());
    ASSERT_STRUCT_FIELD(union_type2, "field1", kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()));
    ASSERT_STRUCT_BITFIELD(union_type2, "field10", kefir_ast_type_unsigned_int(), 5);
    ASSERT_STRUCT_BITFIELD(union_type2, "field11", kefir_ast_type_unsigned_int(), 3);
    ASSERT_STRUCT_BITFIELD(union_type2, "field20", kefir_ast_type_unsigned_long(), 16);
    ASSERT_STRUCT_FIELD(union_type2, "field21", kefir_ast_type_bool());

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

#undef ASSERT_STRUCT_FIELD
#undef ASSERT_STRUCT_BITFIELD

#define ASSERT_FUNCTION_PARAM(_func, _index, _id, _type, _storage)                    \
    do {                                                                              \
        const struct kefir_ast_function_type_parameter *_param = NULL;                \
        ASSERT_OK(kefir_ast_type_function_get_parameter((_func), (_index), &_param)); \
        if (_id != NULL) {                                                            \
            ASSERT((_param)->identifier != NULL);                                     \
            const char *_ident = (_id);                                               \
            ASSERT(strcmp(_param->identifier, _ident) == 0);                          \
        } else {                                                                      \
            ASSERT((_param)->identifier == NULL);                                     \
        }                                                                             \
        if ((_type) != NULL) {                                                        \
            ASSERT(_param->type != NULL);                                             \
            ASSERT(KEFIR_AST_TYPE_SAME(_param->type, (_type)));                       \
        } else {                                                                      \
            ASSERT(_param->type == NULL);                                             \
        }                                                                             \
        if ((_storage) != NULL) {                                                     \
            ASSERT(!KEFIR_OPTIONAL_EMPTY(&_param->storage));                          \
            ASSERT(*KEFIR_OPTIONAL_VALUE(&_param->storage) ==                         \
                   *(const kefir_ast_scoped_identifier_storage_t *) (_storage));      \
        } else {                                                                      \
            ASSERT(KEFIR_OPTIONAL_EMPTY(&_param->storage));                           \
        }                                                                             \
    } while (0)

DEFINE_CASE(ast_type_construction8, "AST Types - function type") {
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));
    const kefir_ast_scoped_identifier_storage_t REGISTER = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER;

    struct kefir_ast_function_type *func_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, &type_bundle, kefir_ast_type_unsigned_long_long(), "func1", &func_type1);
    ASSERT(type1 != NULL);
    ASSERT(func_type1 != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(func_type1->return_type, kefir_ast_type_unsigned_long_long()));
    ASSERT(func_type1->mode == KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY);
    ASSERT(kefir_list_length(&func_type1->parameters) == 0);
    ASSERT(!func_type1->ellipsis);

    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, &type_bundle, func_type1, "arg1",
        kefir_ast_type_unbounded_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(), NULL), NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type1, "arg2", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type1, NULL,
                                                kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()),
                                                &REGISTER));
    ASSERT_OK(kefir_ast_type_function_ellipsis(func_type1, true));

    ASSERT(func_type1->mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS);
    ASSERT(kefir_ast_type_function_parameter_count(func_type1) == 3);
    ASSERT_FUNCTION_PARAM(func_type1, 0, "arg1",
                          kefir_ast_type_unbounded_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(), NULL),
                          NULL);
    ASSERT_FUNCTION_PARAM(func_type1, 1, "arg2", kefir_ast_type_float(), NULL);
    ASSERT_FUNCTION_PARAM(func_type1, 2, NULL, kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()),
                          &REGISTER);
    ASSERT(func_type1->ellipsis);

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_function(&kft_mem, &type_bundle, kefir_ast_type_void(), "func2", &func_type2);
    ASSERT(type2 != NULL);
    ASSERT(func_type2 != NULL);
    ASSERT(KEFIR_AST_TYPE_SAME(func_type2->return_type, kefir_ast_type_void()));
    ASSERT(func_type2->mode == KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY);
    ASSERT(kefir_list_length(&func_type2->parameters) == 0);
    ASSERT(!func_type2->ellipsis);

    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type2, "arg1", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type2, "arg2", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type2, "arg3", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type2, "arg4", NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type2, "arg5", NULL, NULL));

    ASSERT(func_type2->mode == KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS);
    ASSERT(kefir_ast_type_function_parameter_count(func_type2) == 5);
    ASSERT_FUNCTION_PARAM(func_type2, 0, "arg1", NULL, NULL);
    ASSERT_FUNCTION_PARAM(func_type2, 1, "arg2", NULL, NULL);
    ASSERT_FUNCTION_PARAM(func_type2, 2, "arg3", NULL, NULL);
    ASSERT_FUNCTION_PARAM(func_type2, 3, "arg4", NULL, NULL);
    ASSERT_FUNCTION_PARAM(func_type2, 4, "arg5", NULL, NULL);
    ASSERT(!func_type2->ellipsis);

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

#undef ASSERT_FUNCTION_PARAM
