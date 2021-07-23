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

#include <stdio.h>
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/layout.h"
#include "kefir/test/util.h"
#include "kefir/util/json.h"
#include "./translate_type_common.c"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));
    REQUIRE_OK(kefir_json_output_array_begin(&json));

    const struct kefir_ast_type *type1 = kefir_ast_type_signed_long();
    REQUIRE_OK(dump_type(mem, &json, type1));

    const struct kefir_ast_type *type2 = kefir_ast_type_array(mem, &type_bundle, kefir_ast_type_char(),
                                                              kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE_OK(dump_type(mem, &json, type2));

    const struct kefir_ast_type *type3 =
        kefir_ast_type_array(mem, &type_bundle,
                             kefir_ast_type_array(mem, &type_bundle, kefir_ast_type_char(),
                                                  kefir_ast_constant_expression_integer(mem, 5), NULL),
                             kefir_ast_constant_expression_integer(mem, 10), NULL);
    REQUIRE_OK(dump_type(mem, &json, type3));

    struct kefir_ast_enum_type *enum_type_4_0 = NULL;
    const struct kefir_ast_type *type4_0 =
        kefir_ast_type_enumeration(mem, &type_bundle, "", kefir_ast_type_signed_int(), &enum_type_4_0);

    struct kefir_ast_struct_type *struct_type4_1 = NULL;
    const struct kefir_ast_type *type4_1 = kefir_ast_type_structure(mem, &type_bundle, "", &struct_type4_1);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4_1, "field1", kefir_ast_type_char(), NULL));
    REQUIRE_OK(
        kefir_ast_struct_type_field(mem, &symbols, struct_type4_1, "field2",
                                    kefir_ast_type_qualified(mem, &type_bundle, kefir_ast_type_unsigned_long(),
                                                             (struct kefir_ast_type_qualification){.constant = true}),
                                    NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(
        mem, &symbols, struct_type4_1, "field3",
        kefir_ast_type_array(mem, &type_bundle, type4_0, kefir_ast_constant_expression_integer(mem, 15), NULL), NULL));

    struct kefir_ast_struct_type *struct_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_structure(mem, &type_bundle, "", &struct_type4);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "x", kefir_ast_type_float(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "y",
                                           kefir_ast_type_pointer(mem, &type_bundle, kefir_ast_type_double()), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "z", kefir_ast_type_signed_short(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "a",
                                           kefir_ast_type_array(mem, &type_bundle, kefir_ast_type_signed_int(),
                                                                kefir_ast_constant_expression_integer(mem, 1), NULL),
                                           NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, "b", type4_1, NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, &symbols, struct_type4, NULL, type4_1, NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(
        mem, &symbols, struct_type4, "c",
        kefir_ast_type_array(mem, &type_bundle, type4_1, kefir_ast_constant_expression_integer(mem, 10), NULL), NULL));
    REQUIRE_OK(dump_type(mem, &json, type4));

    REQUIRE_OK(kefir_json_output_array_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &type_bundle));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
