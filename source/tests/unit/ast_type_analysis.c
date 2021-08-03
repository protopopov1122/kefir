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
#include "kefir/ast/type_conv.h"

#define ASSERT_TYPE_TAG(type, _tag)    \
    do {                               \
        ASSERT((type) != NULL);        \
        ASSERT((type)->tag == (_tag)); \
    } while (0)

DEFINE_CASE(ast_type_analysis_integer_promotion1, "AST Type analysis - integer promotion") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_bool()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_char()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_signed_char()),
                    KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_unsigned_char()),
                    KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_signed_short()),
                    KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_unsigned_short()),
                    KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_signed_int()),
                    KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_unsigned_int()),
                    KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_signed_long()),
                    KEFIR_AST_TYPE_SCALAR_SIGNED_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_unsigned_long()),
                    KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_signed_long_long()),
                    KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_unsigned_long_long()),
                    KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG);
    ASSERT(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_void()) == NULL);
    ASSERT(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_float()) == NULL);
    ASSERT(kefir_ast_type_int_promotion(type_traits, kefir_ast_type_double()) == NULL);
}
END_CASE

DEFINE_CASE(ast_type_analysis_arithmetic_conversion1, "AST Type analysis - arithmetic conversion #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    const struct kefir_ast_type *TYPES[] = {kefir_ast_type_bool(),           kefir_ast_type_char(),
                                            kefir_ast_type_unsigned_char(),  kefir_ast_type_signed_char(),
                                            kefir_ast_type_unsigned_short(), kefir_ast_type_signed_short()};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES_LEN; j++) {
            ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], TYPES[j]),
                            KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
        }
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], kefir_ast_type_signed_int()),
                        KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], kefir_ast_type_unsigned_int()),
                        KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], kefir_ast_type_signed_long()),
                        KEFIR_AST_TYPE_SCALAR_SIGNED_LONG);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], kefir_ast_type_unsigned_long()),
                        KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], kefir_ast_type_signed_long_long()),
                        KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], kefir_ast_type_unsigned_long_long()),
                        KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG);
    }
}
END_CASE

DEFINE_CASE(ast_type_analysis_arithmetic_conversion2, "AST Type analysis - arithmetic conversion #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    const struct kefir_ast_type *TYPES[] = {kefir_ast_type_bool(),           kefir_ast_type_char(),
                                            kefir_ast_type_unsigned_char(),  kefir_ast_type_signed_char(),
                                            kefir_ast_type_unsigned_short(), kefir_ast_type_signed_short(),
                                            kefir_ast_type_unsigned_int(),   kefir_ast_type_signed_int()};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);
    const struct kefir_ast_type *TYPES2[] = {
        kefir_ast_type_unsigned_long(),    kefir_ast_type_signed_long(), kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(), kefir_ast_type_float(),       kefir_ast_type_double()};
    const kefir_size_t TYPES2_LEN = sizeof(TYPES2) / sizeof(TYPES2[0]);
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES2_LEN; j++) {
            ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], TYPES2[j]), TYPES2[j]->tag);
        }
    }
}
END_CASE

DEFINE_CASE(ast_type_analysis_arithmetic_conversion3, "AST Type analysis - arithmetic conversion #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    const struct kefir_ast_type *TYPES[] = {kefir_ast_type_bool(),
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
                                            kefir_ast_type_float()};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);
    const struct kefir_ast_type *TYPES2[] = {kefir_ast_type_float(), kefir_ast_type_double()};
    const kefir_size_t TYPES2_LEN = sizeof(TYPES2) / sizeof(TYPES2[0]);
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES2_LEN; j++) {
            ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(type_traits, TYPES[i], TYPES2[j]), TYPES2[j]->tag);
        }
    }
}
END_CASE
