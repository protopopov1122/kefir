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

#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

struct kefir_ast_constant *make_constant(struct kefir_mem *, const struct kefir_ast_type *);

#define ASSERT_CONDITIONAL(_mem, _context, _cond, _expr1, _expr2, _const, _checker)       \
    do {                                                                                  \
        struct kefir_ast_conditional_operator *oper =                                     \
            kefir_ast_new_conditional_operator((_mem), (_cond), (_expr1), (_expr2));      \
        ASSERT(oper != NULL);                                                             \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper))); \
        ASSERT(oper->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION);     \
        ASSERT(oper->base.properties.expression_props.constant_expression == (_const));   \
        ASSERT(!oper->base.properties.expression_props.lvalue);                           \
        ASSERT(!oper->base.properties.expression_props.addressable);                      \
        ASSERT(!oper->base.properties.expression_props.bitfield);                         \
        _checker;                                                                         \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper)));                \
    } while (0)

#define ASSERT_CONDITIONAL_NOK(_mem, _context, _cond, _expr1, _expr2)                      \
    do {                                                                                   \
        struct kefir_ast_conditional_operator *oper =                                      \
            kefir_ast_new_conditional_operator((_mem), (_cond), (_expr1), (_expr2));       \
        ASSERT(oper != NULL);                                                              \
        ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper))); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper)));                 \
    } while (0)

DEFINE_CASE(ast_node_analysis_conditional_operator1, "AST node analysis - conditional operator #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type1);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "x", type1, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "y", kefir_ast_type_unsigned_int(),
                                                        NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "X",
                                                      kefir_ast_constant_expression_integer(&kft_mem, 101),
                                                      type_traits->underlying_enumeration_type, NULL, NULL));

    struct kefir_ast_type_name *TYPES[] = {
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)))};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[0]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_bool(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[1]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[1]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_char(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[5]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_unsigned(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[5]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[5]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_signed(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[7]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_float(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[8]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_double(&kft_mem)));

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_OK(kefir_ast_analyze_node(&kft_mem, context, KEFIR_AST_NODE_BASE(TYPES[i])));
        const struct kefir_ast_type *type = TYPES[i]->base.properties.type;
        ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(make_constant(&kft_mem, type->referenced_type)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)), true, {
                               ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type,
                                                          KEFIR_AST_TYPE_IS_CHARACTER(type->referenced_type)
                                                              ? kefir_ast_type_signed_int()
                                                              : type->referenced_type));
                           });

        ASSERT_CONDITIONAL(&kft_mem, context,
                           KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                               &kft_mem, TYPES[i], KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)), true,
                           { ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type, type)); });
    }

    ASSERT_CONDITIONAL_NOK(&kft_mem, context,
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)));

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)), false,
        { ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type, kefir_ast_type_unsigned_int())); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)), true,
        { ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type, type_traits->underlying_enumeration_type)); });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator2, "AST node analysis - conditional operator #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context, "x", kefir_ast_type_signed_int(),
                                                        NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_global_context_define_constant(&kft_mem, &global_context, "X",
                                                       kefir_ast_constant_expression_integer(&kft_mem, 54),
                                                       type_traits->underlying_enumeration_type, NULL, NULL));

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_boolean(),          kefir_ast_type_char(),         kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),      kefir_ast_type_unsigned_int(), kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),    kefir_ast_type_signed_long(),  kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(), kefir_ast_type_float(),        kefir_ast_type_double()};
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES_LEN; j++) {
            ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                               KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
                               KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[j])), true, {
                                   ASSERT(KEFIR_AST_TYPE_SAME(
                                       oper->base.properties.type,
                                       kefir_ast_type_common_arithmetic(context->type_traits, TYPES[i], TYPES[j])));
                               });
        }

        ASSERT_CONDITIONAL(
            &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")), false, {
                ASSERT(KEFIR_AST_TYPE_SAME(
                    oper->base.properties.type,
                    kefir_ast_type_common_arithmetic(context->type_traits, TYPES[i], kefir_ast_type_signed_int())));
            });

        ASSERT_CONDITIONAL(
            &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")), true, {
                ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type,
                                           kefir_ast_type_common_arithmetic(context->type_traits, TYPES[i],
                                                                            type_traits->underlying_enumeration_type)));
            });

        struct kefir_ast_type_name *type_name1 =
            kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                             kefir_ast_type_specifier_void(&kft_mem)));
        ASSERT_CONDITIONAL_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                               KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                                   &kft_mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
                               KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));
    }

    struct kefir_ast_type_name *type_name1 =
        kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem, type_name1,
                                                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, kefir_ast_type_void())); });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator3, "AST node analysis - conditional operator #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type1);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1, "x", kefir_ast_type_signed_int(), NULL));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type2, "y", kefir_ast_type_unsigned_long(),
                                          NULL));

    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem, context->type_bundle, "", &struct_type3);
    ASSERT_OK(
        kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3, "z", kefir_ast_type_double(), NULL));

    struct kefir_ast_struct_type *union_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_union(&kft_mem, context->type_bundle, "", &union_type4);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type4, "z", kefir_ast_type_double(), NULL));

    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "a1", type1, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context, "a2", type1, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "b1", type2, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context, "b2", type2, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "c1", type3, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context, "c2", type3, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context, "d1", type4, NULL, NULL, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context, "d2", type4, NULL, NULL, NULL, NULL));

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type1)); });

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a2")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type1)); });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")));

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type2)); });

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b2")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type2)); });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b2")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")));

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type3)); });

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c2")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type3)); });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a2")));

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type4)); });

    ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")),
                       KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d2")), false,
                       { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type4)); });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator4, "AST node analysis - conditional operator #4") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type_1 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_long());

    struct kefir_ast_type_name *type_name1 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));

    const struct kefir_ast_type *type_2 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    struct kefir_ast_type_name *type_name2 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));

    const struct kefir_ast_type *type_3 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
            (struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = false}));

    struct kefir_ast_type_name *type_name3 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));

    const struct kefir_ast_type *type_4 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = false}));

    struct kefir_ast_type_name *type_name4 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));

    const struct kefir_ast_type *type_5 = kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void());

    struct kefir_ast_type_name *type_name5 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name5->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));

    const struct kefir_ast_type *type_6 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false}));

    struct kefir_ast_type_name *type_name6 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name6->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name6->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));

    const struct kefir_ast_type *type_7 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = false}));

    struct kefir_ast_type_name *type_name7 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name7->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name7->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));

    const struct kefir_ast_type *type_8 = kefir_ast_type_pointer(
        &kft_mem, context->type_bundle,
        kefir_ast_type_qualified(
            &kft_mem, context->type_bundle, kefir_ast_type_void(),
            (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = false}));

    struct kefir_ast_type_name *type_name8 = kefir_ast_new_type_name(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name8->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name8->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name8->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_1)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_2)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_4)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name4))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_4)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_3)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_4)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name5))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_5)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name6))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_6)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name6))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_6)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name6))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_8)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name7))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_7)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name5))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name5))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_5)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name6))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name5))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_6)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name6))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name7))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_8)); });

    ASSERT_CONDITIONAL(
        &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name4))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
            &kft_mem,
            (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(type_name8))->self,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type_8)); });

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name3)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name4)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name5)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name6)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name7)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(type_name8)));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator5, "AST node analysis - conditional operator #5") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_type_name *TYPES[] = {
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
        kefir_ast_new_type_name(
            &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)))};
    const kefir_size_t TYPE_LENGTH = sizeof(TYPES) / sizeof(TYPES[0]);

    const struct kefir_ast_type *TYPES2[] = {
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_long()),
        kefir_ast_type_pointer(
            &kft_mem, context->type_bundle,
            kefir_ast_type_qualified(
                &kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
                (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false})),
        kefir_ast_type_pointer(
            &kft_mem, context->type_bundle,
            kefir_ast_type_qualified(
                &kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
                (struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = false})),
        kefir_ast_type_pointer(
            &kft_mem, context->type_bundle,
            kefir_ast_type_qualified(
                &kft_mem, context->type_bundle, kefir_ast_type_signed_long(),
                (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = false})),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(
            &kft_mem, context->type_bundle,
            kefir_ast_type_qualified(
                &kft_mem, context->type_bundle, kefir_ast_type_void(),
                (struct kefir_ast_type_qualification){.constant = true, .restricted = false, .volatile_type = false})),
        kefir_ast_type_pointer(
            &kft_mem, context->type_bundle,
            kefir_ast_type_qualified(
                &kft_mem, context->type_bundle, kefir_ast_type_void(),
                (struct kefir_ast_type_qualification){.constant = false, .restricted = true, .volatile_type = false})),
        kefir_ast_type_pointer(
            &kft_mem, context->type_bundle,
            kefir_ast_type_qualified(
                &kft_mem, context->type_bundle, kefir_ast_type_void(),
                (struct kefir_ast_type_qualification){.constant = true, .restricted = true, .volatile_type = false}))};

    _Static_assert(sizeof(TYPES) / sizeof(TYPES[0]) == sizeof(TYPES2) / sizeof(TYPES2[0]),
                   "Type array length mismatch");

    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[0]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[1]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[1]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[2]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[2]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[3]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_long(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[4]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[5]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[5]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[6]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[7]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_const(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[7]->type_decl.specifiers,
                                                         kefir_ast_type_qualifier_restrict(&kft_mem)));
    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &TYPES[7]->type_decl.specifiers,
                                                         kefir_ast_type_specifier_void(&kft_mem)));

    for (kefir_size_t i = 0; i < TYPE_LENGTH; i++) {
        ASSERT_CONDITIONAL(
            &kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                &kft_mem,
                (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(&kft_mem, KEFIR_AST_NODE_BASE(TYPES[i]))->self,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)), true,
            { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, TYPES2[i])); });
        ASSERT_CONDITIONAL(&kft_mem, context, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
                           KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
                               &kft_mem, TYPES[i], KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
                           true, { ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, TYPES2[i])); });
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
