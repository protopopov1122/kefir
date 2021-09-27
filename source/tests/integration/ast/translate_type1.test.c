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

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ir/format.h"
#include "kefir/test/util.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ir_type ir_type;
    struct kefir_irbuilder_type builder;
    struct kefir_ast_translator_environment env;
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    REQUIRE_OK(kefir_ir_type_alloc(mem, 0, &ir_type));
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, &ir_type));
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    const struct kefir_ast_type *type1 =
        kefir_ast_type_array(mem, &type_bundle, kefir_ast_type_pointer(mem, &type_bundle, kefir_ast_type_void()),
                             kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE(type1 != NULL, KEFIR_INTERNAL_ERROR);
    REQUIRE_OK(kefir_ast_translate_object_type(mem, type1, 0, &env, &builder, NULL));

    struct kefir_ast_struct_type *struct_type2;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(mem, &type_bundle, "struct1", &struct_type2);
    REQUIRE(type2 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ast_struct_type *union_type3;
    const struct kefir_ast_type *type3 = kefir_ast_type_union(mem, &type_bundle, "union1", &union_type3);
    REQUIRE(type3 != NULL, KEFIR_INTERNAL_ERROR);
    struct kefir_ast_function_type *function_type4 = NULL;
    const struct kefir_ast_type *type4 =
        kefir_ast_type_function(mem, &type_bundle, kefir_ast_type_void(), &function_type4);

    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type2, "field1", kefir_ast_type_float(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(
        mem, &symbols, union_type3, "sub1",
        kefir_ast_type_array(
            mem, &type_bundle,
            kefir_ast_type_qualified(mem, &type_bundle, kefir_ast_type_unsigned_char(),
                                     (const struct kefir_ast_type_qualification){
                                         .constant = true, .restricted = false, .volatile_type = false}),
            kefir_ast_constant_expression_integer(mem, 256), NULL),
        NULL));
    REQUIRE_OK(
        kefir_ast_struct_type_field(mem, &symbols, union_type3, "sub2", kefir_ast_type_unsigned_long_long(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type2, NULL, type3, NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type2, NULL,
                                           kefir_ast_type_pointer(mem, &type_bundle, type4), NULL));
    REQUIRE_OK(kefir_ast_struct_type_bitfield(mem, &symbols, struct_type2, "h1", kefir_ast_type_signed_short(), NULL,
                                              kefir_ast_constant_expression_integer(mem, 6)));
    REQUIRE_OK(kefir_ast_struct_type_bitfield(mem, &symbols, struct_type2, "h2", kefir_ast_type_signed_short(), NULL,
                                              kefir_ast_constant_expression_integer(mem, 1)));
    REQUIRE_OK(kefir_ast_struct_type_bitfield(mem, &symbols, struct_type2, NULL, kefir_ast_type_signed_short(), NULL,
                                              kefir_ast_constant_expression_integer(mem, 0)));
    REQUIRE_OK(kefir_ast_struct_type_bitfield(mem, &symbols, struct_type2, "h10", kefir_ast_type_signed_short(), NULL,
                                              kefir_ast_constant_expression_integer(mem, 3)));
    REQUIRE_OK(kefir_ast_translate_object_type(mem, type2, 0, &env, &builder, NULL));

    REQUIRE_OK(kefir_ir_format_type(stdout, &ir_type));
    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &type_bundle));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    REQUIRE_OK(kefir_ir_type_free(mem, &ir_type));
    return KEFIR_OK;
}
