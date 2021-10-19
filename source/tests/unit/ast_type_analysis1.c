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
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_type_analysis_scalars, "AST type analysis - scalars") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *TYPES[] = {kefir_ast_type_void(),
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
                                            kefir_ast_type_double()};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        const struct kefir_ast_type *type1 = TYPES[i];
        ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));
        ASSERT(KEFIR_AST_TYPE_SAME(TYPES[i], type1));

        type1 = TYPES[i];
        ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type1, NULL));
        ASSERT(KEFIR_AST_TYPE_SAME(TYPES[i], type1));
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_type_analysis_pointers_qualifications, "AST type analysis - pointers & qualifications") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *func_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type1);

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type2);

    struct kefir_ast_struct_type *union_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_union(&kft_mem, context->type_bundle, "", &union_type3);

    const struct kefir_ast_type *TYPES[] = {
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
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle,
                               kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_int())),
        kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(),
                             kefir_ast_constant_expression_integer(&kft_mem, 16), NULL),
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
                                 (struct kefir_ast_type_qualification){.restricted = true}),
        type1,
        type2,
        type3};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        const struct kefir_ast_type *type1 = kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]);
        ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]), type1));

        type1 = kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]);
        ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type1, NULL));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]), type1));

        struct kefir_ast_type_qualification qualification = {.constant = true};
        const struct kefir_ast_type *type2 =
            kefir_ast_type_qualified(&kft_mem, context->type_bundle, TYPES[i], qualification);
        ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2, NULL));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_type_qualified(&kft_mem, context->type_bundle, TYPES[i], qualification),
                                   type2));

        type2 = kefir_ast_type_qualified(&kft_mem, context->type_bundle, TYPES[i], qualification);
        ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type2, NULL));
        ASSERT(KEFIR_AST_TYPE_SAME(kefir_ast_type_qualified(&kft_mem, context->type_bundle, TYPES[i], qualification),
                                   type2));
    }

    const struct kefir_ast_type *type4 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_array_static(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(),
                                    kefir_ast_constant_expression_integer(&kft_mem, 16),
                                    &(struct kefir_ast_type_qualification){.constant = true}));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type4, NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type4, NULL));

    const struct kefir_ast_type *type5 = kefir_ast_type_qualified(
        &kft_mem, context->type_bundle,
        kefir_ast_type_array_static(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(),
                                    kefir_ast_constant_expression_integer(&kft_mem, 16),
                                    &(struct kefir_ast_type_qualification){.constant = true}),
        (struct kefir_ast_type_qualification){.constant = true});
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type5, NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type5, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

#define ASSERT_ENUM_CONSTANT(_enum, _id, _value)                      \
    do {                                                              \
        const struct kefir_ast_constant_expression *_cnst;            \
        ASSERT_OK(kefir_ast_enumeration_get((_enum), (_id), &_cnst)); \
        ASSERT(_cnst->value.integer == (_value));                     \
    } while (0)

DEFINE_CASE(ast_type_analysis_enumerations, "AST type analysis - enumerations") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_enum_type *enum_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(
        &kft_mem, context->type_bundle, "", type_traits->underlying_enumeration_type, &enum_type1);

    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "A"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "B"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "C"));
    ASSERT_OK(kefir_ast_enumeration_type_constant(
        &kft_mem, context->symbols, enum_type1, "X",
        kefir_ast_new_constant_expression(
            &kft_mem,
            KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                &kft_mem, KEFIR_AST_OPERATION_ADD, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 20)))))));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "Y"));
    ASSERT_OK(kefir_ast_enumeration_type_constant_auto(&kft_mem, context->symbols, enum_type1, "Z"));

    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));
    ASSERT_ENUM_CONSTANT(enum_type1, "A", 0);
    ASSERT_ENUM_CONSTANT(enum_type1, "B", 1);
    ASSERT_ENUM_CONSTANT(enum_type1, "C", 2);
    ASSERT_ENUM_CONSTANT(enum_type1, "X", 30);
    ASSERT_ENUM_CONSTANT(enum_type1, "Y", 31);
    ASSERT_ENUM_CONSTANT(enum_type1, "Z", 32);

    struct kefir_ast_enum_type *enum_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_enumeration(&kft_mem, context->type_bundle, "", kefir_ast_type_float(), &enum_type2);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2, NULL));

    const struct kefir_ast_type *type3 = kefir_ast_type_incomplete_enumeration(
        &kft_mem, context->type_bundle, "enum1", type_traits->underlying_enumeration_type);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type3, NULL));

    const struct kefir_ast_type *type4 =
        kefir_ast_type_incomplete_enumeration(&kft_mem, context->type_bundle, "enum2", kefir_ast_type_void());
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type4, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

#undef ASSERT_ENUM_CONSTANT

DEFINE_CASE(ast_type_analysis_arrays, "AST type analysis - arrays") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 = kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_void(),
                                                              kefir_ast_constant_expression_integer(&kft_mem, 1), NULL);
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type1, NULL));

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_array(
        &kft_mem, context->type_bundle,
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_char(), &func_type2),
        kefir_ast_constant_expression_integer(&kft_mem, 2), NULL);
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2, NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type2, NULL));

    const struct kefir_ast_type *type3 =
        kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
                             kefir_ast_constant_expression_integer(&kft_mem, 16), NULL);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type3, NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type3, NULL));

    const struct kefir_ast_type *type4 =
        kefir_ast_type_array_static(&kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
                                    kefir_ast_constant_expression_integer(&kft_mem, 16), NULL);
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type4, NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type4, NULL));

    const struct kefir_ast_type *type5 = kefir_ast_type_array(
        &kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
        kefir_ast_constant_expression_integer(&kft_mem, 16), &(struct kefir_ast_type_qualification){.constant = true});
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type5, NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type5, NULL));

    const struct kefir_ast_type *type6 = kefir_ast_type_array(
        &kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
        kefir_ast_new_constant_expression(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5))),
        NULL);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type6, NULL));
    ASSERT(type6->array_type.const_length->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER);
    ASSERT(type6->array_type.const_length->value.integer == 5);

    const struct kefir_ast_type *type7 = kefir_ast_type_array_static(
        &kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
        kefir_ast_new_constant_expression(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 15))),
        NULL);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, type7, NULL));
    ASSERT(type7->array_type.const_length->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER);
    ASSERT(type7->array_type.const_length->value.integer == 15);

    const struct kefir_ast_type *type8 = kefir_ast_type_array(
        &kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
        kefir_ast_new_constant_expression(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 3.1))),
        NULL);
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type8, NULL));

    const struct kefir_ast_type *type9 =
        kefir_ast_type_vlen_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
                                  KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(&kft_mem, 1.27)), NULL);
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type9, NULL));

    const struct kefir_ast_type *type10 =
        kefir_ast_type_vlen_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_short(),
                                  KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 31)), NULL);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type10, NULL));
    ASSERT(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type10->array_type.vla_length->properties.type));

    const struct kefir_ast_type *type11 =
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_double(), NULL);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type11, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_type_analysis_structs, "AST type analysis - structures/unions") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 = kefir_ast_type_incomplete_structure(&kft_mem, context->type_bundle, "struct1");
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct2", &struct_type2);
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2, NULL));

    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "field1",
                                          kefir_ast_type_signed_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "field2",
                                          kefir_ast_type_signed_short(),
                                          kefir_ast_alignment_as_type(&kft_mem, kefir_ast_type_char())));
    ASSERT_OK(kefir_ast_struct_type_bitfield(
        &kft_mem, context->symbols, struct_type3, "field3", kefir_ast_type_signed_long(), NULL,
        kefir_ast_new_constant_expression(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)))));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "field4",
                                    kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_double(),
                                                         kefir_ast_constant_expression_integer(&kft_mem, 8), NULL),
                                    NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type3, "field5",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL), NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type3, NULL));

    const struct kefir_ast_struct_field *field1 = NULL;
    ASSERT_OK(kefir_ast_struct_type_get_field(struct_type3, "field1", &field1));
    ASSERT(KEFIR_AST_TYPE_SAME(field1->type, kefir_ast_type_signed_char()));
    ASSERT(field1->alignment != NULL && field1->alignment->klass == KEFIR_AST_ALIGNMENT_DEFAULT);
    ASSERT(field1->alignment->value == KEFIR_AST_DEFAULT_ALIGNMENT);
    ASSERT(!field1->bitfield && field1->bitwidth == NULL);

    const struct kefir_ast_struct_field *field2 = NULL;
    ASSERT_OK(kefir_ast_struct_type_get_field(struct_type3, "field2", &field2));
    ASSERT(KEFIR_AST_TYPE_SAME(field2->type, kefir_ast_type_signed_short()));
    ASSERT(field2->alignment != NULL && field2->alignment->klass == KEFIR_AST_ALIGNMENT_AS_TYPE);
    ASSERT(field2->alignment->value == 1);
    ASSERT(!field2->bitfield && field2->bitwidth == NULL);

    const struct kefir_ast_struct_field *field3 = NULL;
    ASSERT_OK(kefir_ast_struct_type_get_field(struct_type3, "field3", &field3));
    ASSERT(KEFIR_AST_TYPE_SAME(field3->type, kefir_ast_type_signed_long()));
    ASSERT(field3->alignment != NULL && field3->alignment->klass == KEFIR_AST_ALIGNMENT_DEFAULT);
    ASSERT(field3->alignment->value == KEFIR_AST_DEFAULT_ALIGNMENT);
    ASSERT(field3->bitfield && field3->bitwidth != NULL);
    ASSERT(field3->bitwidth->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER);
    ASSERT(field3->bitwidth->value.integer == 2);

    const struct kefir_ast_struct_field *field4 = NULL;
    ASSERT_OK(kefir_ast_struct_type_get_field(struct_type3, "field4", &field4));
    ASSERT(KEFIR_AST_TYPE_SAME(field4->type,
                               kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_double(),
                                                    kefir_ast_constant_expression_integer(&kft_mem, 8), NULL)));
    ASSERT(field4->alignment != NULL && field4->alignment->klass == KEFIR_AST_ALIGNMENT_DEFAULT);
    ASSERT(field4->alignment->value == KEFIR_AST_DEFAULT_ALIGNMENT);
    ASSERT(!field4->bitfield && field4->bitwidth == NULL);

    const struct kefir_ast_struct_field *field5 = NULL;
    ASSERT_OK(kefir_ast_struct_type_get_field(struct_type3, "field5", &field5));
    ASSERT(KEFIR_AST_TYPE_SAME(
        field5->type, kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL)));
    ASSERT(field5->alignment != NULL && field5->alignment->klass == KEFIR_AST_ALIGNMENT_DEFAULT);
    ASSERT(field5->alignment->value == KEFIR_AST_DEFAULT_ALIGNMENT);
    ASSERT(!field5->bitfield && field5->bitwidth == NULL);

    struct kefir_ast_struct_type *struct_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type4);
    ASSERT_OK(kefir_ast_struct_type_bitfield(
        &kft_mem, context->symbols, struct_type4, "x", kefir_ast_type_signed_long(), NULL,
        kefir_ast_new_constant_expression(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -1)))));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type4, NULL));

    struct kefir_ast_struct_type *struct_type5 = NULL;
    const struct kefir_ast_type *type5 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type5);
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type5, "a",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type5, "b", kefir_ast_type_boolean(), NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type5, NULL));

    struct kefir_ast_struct_type *struct_type6 = NULL;
    const struct kefir_ast_type *type6 = kefir_ast_type_union(&kft_mem, context->type_bundle, NULL, &struct_type6);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type6, "union_field_one",
                                          kefir_ast_type_boolean(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type6, "union_field_two",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL), NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type6, NULL));

    struct kefir_ast_struct_type *struct_type7 = NULL;
    const struct kefir_ast_type *type7 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type7);
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type7, "ABC",
        kefir_ast_type_vlen_array(&kft_mem, context->type_bundle, kefir_ast_type_float(),
                                  KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)), NULL),
        NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type7, NULL));

    struct kefir_ast_struct_type *struct_type8 = NULL;
    const struct kefir_ast_type *type8 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type8);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type8, "void", kefir_ast_type_void(), NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type8, NULL));

    struct kefir_ast_function_type *func_type9 = NULL;
    const struct kefir_ast_type *type9f =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type9);
    struct kefir_ast_struct_type *struct_type9 = NULL;
    const struct kefir_ast_type *type9 = kefir_ast_type_structure(&kft_mem, context->type_bundle, NULL, &struct_type9);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type9, "function", type9f, NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type9, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_type_analysis_functions, "AST type analysis - functions") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_function_type *func_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_function(&kft_mem, context->type_bundle,
                                kefir_ast_type_array(&kft_mem, context->type_bundle, kefir_ast_type_char(),
                                                     kefir_ast_constant_expression_integer(&kft_mem, 1), NULL),
                                &func_type1);
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));

    struct kefir_ast_function_type *func_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_function(&kft_mem, context->type_bundle, type1, &func_type2);
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2, NULL));

    struct kefir_ast_function_type *func_type3 = NULL;
    const struct kefir_ast_type *type3 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type3);
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type3, kefir_ast_type_void(), NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type3, NULL));

    struct kefir_ast_function_type *func_type4 = NULL;
    const struct kefir_ast_type *type4 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type4);
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type4, kefir_ast_type_void(), NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type4, NULL));

    struct kefir_ast_function_type *func_type5 = NULL;
    const struct kefir_ast_type *type5 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type5);
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type5, kefir_ast_type_void(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type5,
                                                kefir_ast_type_signed_long(), NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type5, NULL));

    const struct kefir_ast_type *struct_type6 =
        kefir_ast_type_incomplete_structure(&kft_mem, context->type_bundle, "S");
    struct kefir_ast_function_type *func_type6 = NULL;
    const struct kefir_ast_type *type6 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type6);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type6, struct_type6, NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type6, NULL));

    struct kefir_ast_struct_type *structure7 = NULL;
    const struct kefir_ast_type *struct_type7 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "S2", &structure7);
    struct kefir_ast_function_type *func_type7 = NULL;
    const struct kefir_ast_type *type7 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type7);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type7, struct_type7, NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type7, NULL));

    struct kefir_ast_function_type *func_type8 = NULL;
    const struct kefir_ast_type *type8 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type8);
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, context->type_bundle, func_type8,
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_boolean(), NULL), NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type8, NULL));

    struct kefir_ast_function_type *func_type9 = NULL;
    const struct kefir_ast_type *type9 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type9);
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, context->type_bundle, func_type9, kefir_ast_type_signed_int(),
        &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO}));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type9, NULL));

    struct kefir_ast_function_type *func_type10 = NULL;
    const struct kefir_ast_type *type10 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type10);
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, context->type_bundle, func_type10, kefir_ast_type_signed_int(),
        &(kefir_ast_scoped_identifier_storage_t){KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER}));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type10, NULL));

    struct kefir_ast_function_type *func_type11 = NULL;
    const struct kefir_ast_type *type11 =
        kefir_ast_type_function(&kft_mem, context->type_bundle, kefir_ast_type_void(), &func_type11);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type11,
                                                kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(
        kefir_ast_type_function_parameter(&kft_mem, context->type_bundle, func_type11, kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_type_function_parameter(
        &kft_mem, context->type_bundle, func_type11,
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type11, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_type_analysis_flexible_arrays, "AST type analysis - flexible array members") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct1", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "field1",
                                          kefir_ast_type_signed_short(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type1, "field2",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL), NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type1, NULL));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct2", &struct_type2);
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type2, "field1",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL), NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2, NULL));

    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct3", &struct_type3);
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type3, "field1",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_char(), NULL), NULL));
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "field2", kefir_ast_type_double(), NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type3, NULL));

    struct kefir_ast_struct_type *union_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_union(&kft_mem, context->type_bundle, "union4", &union_type4);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type4, "field1",
                                          kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, union_type4, "field2",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_signed_short(), NULL), NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type4, NULL));

    struct kefir_ast_struct_type *struct_type5 = NULL;
    const struct kefir_ast_type *type5 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct5", &struct_type5);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type5, "inner1",
                                          kefir_ast_type_signed_short(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type5, "inner2",
                                          kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
                                          NULL));
    ASSERT_OK(kefir_ast_struct_type_field(
        &kft_mem, context->symbols, struct_type5, "inner3",
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(), NULL), NULL));

    struct kefir_ast_struct_type *struct_type6 = NULL;
    const struct kefir_ast_type *type6 =
        kefir_ast_type_structure(&kft_mem, context->type_bundle, "struct6", &struct_type6);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type6, "outer1",
                                          kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type6, "inner2", type5, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type6, "inner3",
                                          kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, type1, NULL),
                                          NULL));
    ASSERT_OK(kefir_ast_analyze_type(&kft_mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type6, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
