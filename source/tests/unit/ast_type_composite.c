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
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/context.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_type_basic_composite, "AST types - basic composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *BASIC_TYPES[] = {kefir_ast_type_void(),
                                                  kefir_ast_type_boolean(),
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
                                                  kefir_ast_type_long_double(),
                                                  kefir_ast_type_va_list()};

    const kefir_size_t BASIC_TYPES_LENGTH = sizeof(BASIC_TYPES) / sizeof(BASIC_TYPES[0]);
    for (kefir_size_t i = 0; i < BASIC_TYPES_LENGTH; i++) {
        for (kefir_size_t j = 0; j < BASIC_TYPES_LENGTH; j++) {
            if (i == j) {
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASIC_TYPES[i], BASIC_TYPES[j]));
                const struct kefir_ast_type *composite =
                    KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, BASIC_TYPES[i], BASIC_TYPES[j]);
                ASSERT(composite != NULL);
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASIC_TYPES[i], composite));
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite, BASIC_TYPES[i]));
            } else {
                ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASIC_TYPES[i], BASIC_TYPES[j]));
                const struct kefir_ast_type *composite =
                    KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, BASIC_TYPES[i], BASIC_TYPES[j]);
                ASSERT(composite == NULL);
            }
        }
    }

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_type_pointer_composite, "AST types - pointer composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *BASE_TYPES[] = {
        kefir_ast_type_void(),
        kefir_ast_type_boolean(),
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
        kefir_ast_type_long_double(),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_boolean()),
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
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_long_double())};

    const kefir_size_t BASE_TYPES_LENGTH = sizeof(BASE_TYPES) / sizeof(BASE_TYPES[0]);
    for (kefir_size_t i = 0; i < BASE_TYPES_LENGTH; i++) {
        for (kefir_size_t j = 0; j < BASE_TYPES_LENGTH; j++) {
            if (i == j) {
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASE_TYPES[i], BASE_TYPES[j]));
                const struct kefir_ast_type *composite =
                    KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, BASE_TYPES[i], BASE_TYPES[j]);
                ASSERT(composite != NULL);
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASE_TYPES[i], composite));
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite, BASE_TYPES[i]));
            } else {
                ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASE_TYPES[i], BASE_TYPES[j]));
                const struct kefir_ast_type *composite =
                    KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, BASE_TYPES[i], BASE_TYPES[j]);
                ASSERT(composite == NULL);
            }
        }
    }

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

static const struct kefir_ast_type *build_enum_type(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                                    struct kefir_ast_type_bundle *type_bundle,
                                                    const struct kefir_ast_type_traits *type_traits,
                                                    struct kefir_ast_enum_type **enum1_type) {
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(
        &kft_mem, type_bundle, "enum1", type_traits->underlying_enumeration_type, enum1_type);
    ASSERT_OK(kefir_ast_enumeration_type_constant(mem, symbols, *enum1_type, "c1",
                                                  kefir_ast_constant_expression_integer(mem, 10)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(mem, symbols, *enum1_type, "c2"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(mem, symbols, *enum1_type, "c3"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(mem, symbols, *enum1_type, "c4",
                                                  kefir_ast_constant_expression_integer(mem, 1)));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(mem, symbols, *enum1_type, "c5"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(mem, symbols, *enum1_type, "c10",
                                                  kefir_ast_constant_expression_integer(mem, 0)));
    return type1;
}

DEFINE_CASE(ast_type_enum_composite, "AST types - enum composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_enum_type *enum1_type;
    const struct kefir_ast_type *enum1 = build_enum_type(&kft_mem, &symbols, &type_bundle, type_traits, &enum1_type);
    struct kefir_ast_enum_type *enum2_type;
    const struct kefir_ast_type *enum2 = build_enum_type(&kft_mem, &symbols, &type_bundle, type_traits, &enum2_type);
    struct kefir_ast_enum_type *enum3_type;
    const struct kefir_ast_type *enum3 = build_enum_type(&kft_mem, &symbols, &type_bundle, type_traits, &enum3_type);
    ASSERT_OK(kefir_ast_enumeration_type_constant(&kft_mem, &symbols, enum3_type, "c20",
                                                  kefir_ast_constant_expression_integer(&kft_mem, 1000)));

    const struct kefir_ast_type *composite12 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, enum1, enum2);
    ASSERT(composite12 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum1, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum2, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, enum1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, enum2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum3, composite12));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, enum3));

    const struct kefir_ast_type *composite21 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, enum2, enum1);
    ASSERT(composite21 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, composite21));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, composite12));

    const struct kefir_ast_type *composite13 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, enum1, enum3);
    ASSERT(composite13 == NULL);
    const struct kefir_ast_type *composite31 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, enum3, enum1);
    ASSERT(composite31 == NULL);
    const struct kefir_ast_type *composite23 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, enum2, enum3);
    ASSERT(composite23 == NULL);
    const struct kefir_ast_type *composite32 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, enum3, enum2);
    ASSERT(composite32 == NULL);

    const struct kefir_ast_type *composite1u = KEFIR_AST_TYPE_COMPOSITE(
        &kft_mem, &type_bundle, type_traits, enum1, kefir_ast_enumeration_underlying_type(enum1_type));
    ASSERT(composite1u != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum1, composite1u));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite1u, enum1));

    const struct kefir_ast_type *composite2u = KEFIR_AST_TYPE_COMPOSITE(
        &kft_mem, &type_bundle, type_traits, enum2, kefir_ast_enumeration_underlying_type(enum2_type));
    ASSERT(composite2u != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum2, composite2u));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite2u, enum2));

    const struct kefir_ast_type *composite3u = KEFIR_AST_TYPE_COMPOSITE(
        &kft_mem, &type_bundle, type_traits, enum3, kefir_ast_enumeration_underlying_type(enum3_type));
    ASSERT(composite3u != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, enum3, composite3u));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite3u, enum3));

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_type_qualified_composite, "AST types - qualified composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *BASE_TYPES[] = {
        kefir_ast_type_void(),
        kefir_ast_type_boolean(),
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
        kefir_ast_type_long_double(),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_boolean()),
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
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_long_double())};
    const kefir_size_t BASE_TYPES_LENGTH = sizeof(BASE_TYPES) / sizeof(BASE_TYPES[0]);

    struct kefir_ast_type_qualification QUALIFICATIONS[] = {
        {.constant = false, .restricted = false, .volatile_type = false},
        {.constant = true, .restricted = false, .volatile_type = false},
        {.constant = false, .restricted = true, .volatile_type = false},
        {.constant = true, .restricted = true, .volatile_type = false},
        {.constant = false, .restricted = false, .volatile_type = true},
        {.constant = true, .restricted = false, .volatile_type = true},
        {.constant = false, .restricted = true, .volatile_type = true},
        {.constant = true, .restricted = true, .volatile_type = true}};
    const kefir_size_t QUALIFICATIONS_LENGTH = sizeof(QUALIFICATIONS) / sizeof(QUALIFICATIONS[0]);

    for (kefir_size_t i = 0; i < BASE_TYPES_LENGTH; i++) {
        for (kefir_size_t j = 0; j < QUALIFICATIONS_LENGTH; j++) {
            const struct kefir_ast_type *qualified1 =
                kefir_ast_type_qualified(&kft_mem, &type_bundle, BASE_TYPES[i], QUALIFICATIONS[j]);
            if (KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&QUALIFICATIONS[j])) {
                const struct kefir_ast_type *composite =
                    KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, qualified1, BASE_TYPES[i]);
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, qualified1, BASE_TYPES[i]));
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, BASE_TYPES[i], qualified1));
                ASSERT(composite != NULL);
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, qualified1, composite));
                ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite, qualified1));
            }
            for (kefir_size_t k = 0; k < QUALIFICATIONS_LENGTH; k++) {
                const struct kefir_ast_type *qualified2 =
                    kefir_ast_type_qualified(&kft_mem, &type_bundle, BASE_TYPES[i], QUALIFICATIONS[k]);
                const struct kefir_ast_type *composite =
                    KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, qualified1, qualified2);
                if (j == k) {
                    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, qualified1, qualified2));
                    ASSERT(composite != NULL);
                    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, qualified1, composite));
                    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, qualified2, composite));
                    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite, qualified1));
                    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite, qualified2));
                } else {
                    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, qualified1, qualified2));
                    ASSERT(composite == NULL);
                }
            }
        }
    }

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

static const struct kefir_ast_type *build_struct_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                      const char *id, struct kefir_ast_struct_type **struct_type) {
    struct kefir_ast_alignment *align1 = kefir_ast_alignment_as_type(mem, kefir_ast_type_unsigned_long_long());
    ASSERT_OK(kefir_ast_analyze_alignment(mem, context, align1));
    ASSERT_OK(kefir_ast_alignment_evaluate(mem, context, align1));

    const struct kefir_ast_type *type = kefir_ast_type_structure(mem, context->type_bundle, id, struct_type);
    ASSERT(type != NULL);
    ASSERT_OK(kefir_ast_struct_type_field(mem, context->symbols, *struct_type, "field1",
                                          kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void()),
                                          NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(mem, context->symbols, *struct_type, "field2", kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(mem, context->symbols, *struct_type, "field3",
                                             kefir_ast_type_signed_long_long(), NULL,
                                             kefir_ast_constant_expression_integer(mem, 10)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(mem, context->symbols, *struct_type, "field4",
                                             kefir_ast_type_signed_long_long(), NULL,
                                             kefir_ast_constant_expression_integer(mem, 2)));
    ASSERT_OK(kefir_ast_struct_type_field(mem, context->symbols, *struct_type, "field5", kefir_ast_type_unsigned_int(),
                                          align1));
    return type;
}

DEFINE_CASE(ast_type_struct_composite, "AST types - structure composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = build_struct_type(&kft_mem, context, "struct1", &struct_type1);
    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = build_struct_type(&kft_mem, context, "struct1", &struct_type2);
    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 = build_struct_type(&kft_mem, context, "struct2", &struct_type3);
    struct kefir_ast_struct_type *struct_type4 = NULL;
    const struct kefir_ast_type *type4 = build_struct_type(&kft_mem, context, "struct1", &struct_type4);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type4, "field1010",
                                          kefir_ast_type_signed_int(), NULL));

    const struct kefir_ast_type *composite12 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type1, type2);
    ASSERT(composite12 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type2, composite12));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite12));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, type4, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type4));

    const struct kefir_ast_type *composite21 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type2, type1);
    ASSERT(composite21 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite21));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type2, composite21));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite21));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, type4, composite21));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type4));

    const struct kefir_ast_type *composite13 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type1, type3);
    ASSERT(composite13 == NULL);
    const struct kefir_ast_type *composite31 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type3, type1);
    ASSERT(composite31 == NULL);
    const struct kefir_ast_type *composite23 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type2, type3);
    ASSERT(composite23 == NULL);
    const struct kefir_ast_type *composite32 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type3, type2);
    ASSERT(composite32 == NULL);
    const struct kefir_ast_type *composite14 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type1, type4);
    ASSERT(composite14 == NULL);
    const struct kefir_ast_type *composite41 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type4, type1);
    ASSERT(composite41 == NULL);
    const struct kefir_ast_type *composite24 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type2, type4);
    ASSERT(composite24 == NULL);
    const struct kefir_ast_type *composite42 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, context->type_bundle, type_traits, type4, type2);
    ASSERT(composite42 == NULL);

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_type_union_composite, "AST types - union composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_struct_type *union_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_union(&kft_mem, &type_bundle, "union1", &union_type1);
    ASSERT(type1 != NULL);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field2",
                                          kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()),
                                          NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type1, "field3",
                                    kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 128), NULL),
                                    NULL));

    struct kefir_ast_struct_type *union_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_union(&kft_mem, &type_bundle, "union1", &union_type2);
    ASSERT(type2 != NULL);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type2, "field3",
                                    kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 128), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type2, "field2",
                                          kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()),
                                          NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type2, "field1", kefir_ast_type_signed_int(), NULL));

    struct kefir_ast_struct_type *union_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_union(&kft_mem, &type_bundle, "union1", &union_type3);
    ASSERT(type3 != NULL);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type3, "field3",
                                    kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 128), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, union_type3, "field2",
                                          kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_float()),
                                          NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type3, "field1", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, &symbols, union_type3, "field0", kefir_ast_type_unsigned_char(), NULL));

    const struct kefir_ast_type *composite12 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type2);
    ASSERT(composite12 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type2, composite12));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type3));

    const struct kefir_ast_type *composite21 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type2, type1);
    ASSERT(composite21 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite21));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type2, composite21));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite21));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type3));

    const struct kefir_ast_type *composite13 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type3);
    ASSERT(composite13 == NULL);
    const struct kefir_ast_type *composite23 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type2, type3);
    ASSERT(composite23 == NULL);
    const struct kefir_ast_type *composite31 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type3, type1);
    ASSERT(composite31 == NULL);
    const struct kefir_ast_type *composite32 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type3, type2);
    ASSERT(composite32 == NULL);

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_type_array_composite, "AST types - array composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 =
        kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 100), NULL);
    const struct kefir_ast_type *type2 =
        kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(),
                             kefir_ast_constant_expression_integer(&kft_mem, 101), NULL);
    const struct kefir_ast_type *type3 =
        kefir_ast_type_unbounded_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(), NULL);
    const struct kefir_ast_type *type4 =
        kefir_ast_type_vlen_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(), NULL, NULL);
    const struct kefir_ast_type *type5 = kefir_ast_type_array_static(
        &kft_mem, &type_bundle, kefir_ast_type_unsigned_char(), kefir_ast_constant_expression_integer(&kft_mem, 100),
        &(const struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = false});

    const struct kefir_ast_type *composite13 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type3);
    ASSERT(composite13 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite13));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite13));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite13, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite13, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite13, type2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite13, type4));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite13, type5));

    const struct kefir_ast_type *composite23 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type2, type3);
    ASSERT(composite23 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type2, composite23));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite23));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite23, type2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite23, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite23, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite23, type4));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite23, type5));

    const struct kefir_ast_type *composite33 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type3, type3);
    ASSERT(composite33 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite33));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite33, type3));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite33, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite33, type2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite33, type4));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite33, type5));

    const struct kefir_ast_type *composite34 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type3, type4);
    ASSERT(composite34 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite34));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type4, composite34));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite34, type3));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite34, type4));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite34, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite34, type2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite34, type5));

    const struct kefir_ast_type *composite35 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type3, type5);
    ASSERT(composite35 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type3, composite35));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type5, composite35));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite35, type3));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite35, type5));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite35, type1));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite35, type2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite35, type4));

    const struct kefir_ast_type *composite15 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type5);
    ASSERT(composite15 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite15));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type5, composite15));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type5));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type4));

    const struct kefir_ast_type *composite12 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type2);
    ASSERT(composite12 == NULL);
    const struct kefir_ast_type *composite25 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type2, type5);
    ASSERT(composite25 == NULL);

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

static const struct kefir_ast_type *build_function_type(struct kefir_mem *mem,
                                                        struct kefir_ast_type_bundle *type_bundle,
                                                        const struct kefir_ast_type *return_type,
                                                        struct kefir_ast_function_type **function_type) {
    const struct kefir_ast_type *type = kefir_ast_type_function(mem, type_bundle, return_type, function_type);
    ASSERT(type != NULL);
    ASSERT(*function_type != NULL);
    ASSERT_OK(kefir_ast_type_function_parameter(mem, type_bundle, *function_type, kefir_ast_type_double(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(
        mem, type_bundle, *function_type,
        kefir_ast_type_array_static(mem, type_bundle, kefir_ast_type_char(),
                                    kefir_ast_constant_expression_integer(&kft_mem, 140), NULL),
        NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(
        mem, type_bundle, *function_type,
        kefir_ast_type_pointer(
            mem, type_bundle,
            kefir_ast_type_qualified(
                mem, type_bundle, kefir_ast_type_unsigned_char(),
                (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false})),
        NULL));

    return kefir_ast_type_qualified(
        mem, type_bundle, kefir_ast_type_pointer(mem, type_bundle, type),
        (const struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false});
}

static const struct kefir_ast_type *build_function_type2(struct kefir_mem *mem,
                                                         struct kefir_ast_type_bundle *type_bundle,
                                                         const struct kefir_ast_type *return_type,
                                                         struct kefir_ast_function_type **function_type) {
    const struct kefir_ast_type *type = kefir_ast_type_function(mem, type_bundle, return_type, function_type);
    ASSERT(type != NULL);
    ASSERT(*function_type != NULL);
    ASSERT_OK(kefir_ast_type_function_parameter(mem, type_bundle, *function_type, NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(mem, type_bundle, *function_type, NULL, NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(mem, type_bundle, *function_type, NULL, NULL));

    return kefir_ast_type_qualified(
        mem, type_bundle, kefir_ast_type_pointer(mem, type_bundle, type),
        (const struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false});
}

DEFINE_CASE(ast_type_function_composite, "AST types - function composite type") {
    const struct kefir_ast_type_traits *type_traits = kefir_util_default_type_traits();
    struct kefir_ast_type_bundle type_bundle;
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_function_type *function_type1 = NULL;
    const struct kefir_ast_type *type1 =
        build_function_type(&kft_mem, &type_bundle, kefir_ast_type_void(), &function_type1);
    struct kefir_ast_function_type *function_type2 = NULL;
    const struct kefir_ast_type *type2 =
        build_function_type(&kft_mem, &type_bundle, kefir_ast_type_void(), &function_type2);
    struct kefir_ast_function_type *function_type3 = NULL;
    const struct kefir_ast_type *type3 = build_function_type(
        &kft_mem, &type_bundle, kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_signed_int()),
        &function_type3);
    struct kefir_ast_function_type *function_type4 = NULL;
    const struct kefir_ast_type *type4 =
        build_function_type(&kft_mem, &type_bundle, kefir_ast_type_void(), &function_type4);
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, &type_bundle, function_type4, kefir_ast_type_signed_int(), NULL));
    struct kefir_ast_function_type *function_type5 = NULL;
    const struct kefir_ast_type *type5 =
        build_function_type2(&kft_mem, &type_bundle, kefir_ast_type_void(), &function_type5);

    const struct kefir_ast_type *composite12 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type2);
    ASSERT(composite12 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type2, composite12));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type4));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite12, type5));

    const struct kefir_ast_type *composite21 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type2, type1);
    ASSERT(composite21 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type2, composite21));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite21));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type2));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type1));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type4));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite21, type5));

    const struct kefir_ast_type *composite15 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type5);
    ASSERT(composite15 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite15));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type5, composite15));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type5));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite15, type4));

    const struct kefir_ast_type *composite51 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type5, type1);
    ASSERT(composite51 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type1, composite51));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type5, composite51));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite51, type1));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite51, type5));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite51, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite51, type3));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite51, type4));

    const struct kefir_ast_type *composite13 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type3);
    ASSERT(composite13 == NULL);
    const struct kefir_ast_type *composite14 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type1, type4);
    ASSERT(composite14 == NULL);
    const struct kefir_ast_type *composite23 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type2, type3);
    ASSERT(composite23 == NULL);
    const struct kefir_ast_type *composite24 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type2, type4);
    ASSERT(composite24 == NULL);
    const struct kefir_ast_type *composite53 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type5, type3);
    ASSERT(composite53 == NULL);
    const struct kefir_ast_type *composite54 =
        KEFIR_AST_TYPE_COMPOSITE(&kft_mem, &type_bundle, type_traits, type5, type4);
    ASSERT(composite54 != NULL);
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type4, composite54));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, type5, composite54));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite54, type4));
    ASSERT(KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite54, type5));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite54, type1));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite54, type2));
    ASSERT(!KEFIR_AST_TYPE_COMPATIBLE(type_traits, composite51, type3));

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
