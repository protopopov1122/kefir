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
#include "kefir/ast/type.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_type_variably_modified1, "AST types - variably modified types #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_void()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_boolean()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_char()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_unsigned_char()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_signed_char()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_unsigned_short()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_signed_short()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_unsigned_int()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_signed_int()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_unsigned_long()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_signed_long()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_unsigned_long_long()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_signed_long_long()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_float()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_double()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_long_double()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_va_list()));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void())));
    ASSERT(!kefir_ast_type_is_variably_modified(kefir_ast_type_qualified(
        &kft_mem, &type_bundle, kefir_ast_type_char(), (struct kefir_ast_type_qualification){.constant = true})));
    ASSERT(!kefir_ast_type_is_variably_modified(
        kefir_ast_type_array(&kft_mem, &type_bundle, kefir_ast_type_signed_int(),
                             kefir_ast_constant_expression_integer(&kft_mem, 10), NULL)));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, &type_bundle, NULL, &struct_type1);
    ASSERT(!kefir_ast_type_is_variably_modified(type1));

    struct kefir_ast_struct_type *union_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_union(&kft_mem, &type_bundle, NULL, &union_type2);
    ASSERT(!kefir_ast_type_is_variably_modified(type2));

    struct kefir_ast_enum_type *enum_type3 = NULL;
    const struct kefir_ast_type *type3 =
        kefir_ast_type_enumeration(&kft_mem, &type_bundle, NULL, kefir_ast_type_signed_int(), &enum_type3);
    ASSERT(!kefir_ast_type_is_variably_modified(type3));

    struct kefir_ast_function_type *func_type4 = NULL;
    const struct kefir_ast_type *type4 =
        kefir_ast_type_function(&kft_mem, &type_bundle, kefir_ast_type_void(), &func_type4);
    ASSERT(!kefir_ast_type_is_variably_modified(type4));

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_type_variably_modified2, "AST types - variably modified types #2") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 =
        kefir_ast_type_vlen_array(&kft_mem, &type_bundle, kefir_ast_type_unsigned_short(),
                                  KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type1));

    const struct kefir_ast_type *type2 =
        kefir_ast_type_vlen_array_static(&kft_mem, &type_bundle, kefir_ast_type_unsigned_char(),
                                         KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "y")), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type2));

    const struct kefir_ast_type *type3 = kefir_ast_type_pointer(&kft_mem, &type_bundle, type1);
    ASSERT(kefir_ast_type_is_variably_modified(type3));

    const struct kefir_ast_type *type4 =
        kefir_ast_type_array(&kft_mem, &type_bundle, type2, kefir_ast_constant_expression_integer(&kft_mem, 15), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type4));

    const struct kefir_ast_type *type5 = kefir_ast_type_pointer(&kft_mem, &type_bundle, type3);
    ASSERT(kefir_ast_type_is_variably_modified(type5));

    struct kefir_ast_struct_type *struct_type6 = NULL;
    const struct kefir_ast_type *type6 = kefir_ast_type_structure(&kft_mem, &type_bundle, NULL, &struct_type6);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type6, "field1", type1, NULL));
    ASSERT(!kefir_ast_type_is_variably_modified(type6));

    struct kefir_ast_struct_type *struct_type7 = NULL;
    const struct kefir_ast_type *type7 = kefir_ast_type_union(&kft_mem, &type_bundle, NULL, &struct_type7);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &symbols, struct_type7, "field1", type2, NULL));
    ASSERT(!kefir_ast_type_is_variably_modified(type7));

    struct kefir_ast_function_type *func_type8 = NULL;
    const struct kefir_ast_type *type8 =
        kefir_ast_type_function(&kft_mem, &type_bundle, kefir_ast_type_void(), &func_type8);
    ASSERT_OK(kefir_ast_type_function_parameter(&kft_mem, &type_bundle, func_type8, type3, NULL));
    ASSERT(!kefir_ast_type_is_variably_modified(type8));

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_type_variably_modified_analysis1, "AST types - variably modified types analysis #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context, NULL));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));

    const struct kefir_ast_type *type1 = kefir_ast_type_vlen_array(
        &kft_mem, &global_context.type_bundle, kefir_ast_type_unsigned_short(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &global_context.symbols, "x")), NULL);
    ASSERT(kefir_ast_type_is_variably_modified(type1));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 =
        kefir_ast_type_structure(&kft_mem, &global_context.type_bundle, NULL, &struct_type2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, &global_context.symbols, struct_type2, "field1", type1, NULL));
    ASSERT_NOK(kefir_ast_analyze_type(&kft_mem, &local_context.context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type2, NULL));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
