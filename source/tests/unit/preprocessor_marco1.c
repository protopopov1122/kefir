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

#include "kefir/preprocessor/macro.h"
#include "kefir/test/unit_test.h"
#include <string.h>

DEFINE_CASE(preprocessor_macro_construction1, "Preprocessor - macro construction") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_preprocessor_user_macro *macro1 =
        kefir_preprocessor_user_macro_new_function(&kft_mem, &symbols, "macro1");
    ASSERT(macro1 != NULL);
    ASSERT(macro1->macro.identifier != NULL);
    ASSERT(strcmp(macro1->macro.identifier, "macro1") == 0);
    ASSERT(!macro1->vararg);
    ASSERT(macro1->replacement.length == 0);

    ASSERT_OK(kefir_list_insert_after(&kft_mem, &macro1->parameters, kefir_list_tail(&macro1->parameters), "p1"));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &macro1->parameters, kefir_list_tail(&macro1->parameters), "p2"));
    ASSERT_OK(kefir_list_insert_after(&kft_mem, &macro1->parameters, kefir_list_tail(&macro1->parameters), "p3"));
    macro1->vararg = true;

    struct kefir_token token;
    ASSERT_OK(kefir_token_new_pp_number(&kft_mem, "123", 3, &token));
    ASSERT_OK(kefir_token_buffer_emplace(&kft_mem, &macro1->replacement, &token));
    ASSERT_OK(kefir_token_new_pp_number(&kft_mem, "3210", 4, &token));
    ASSERT_OK(kefir_token_buffer_emplace(&kft_mem, &macro1->replacement, &token));

    ASSERT_OK(kefir_preprocessor_user_macro_free(&kft_mem, macro1));

    struct kefir_preprocessor_user_macro *macro2 =
        kefir_preprocessor_user_macro_new_object(&kft_mem, &symbols, "macro2");
    ASSERT(macro2 != NULL);
    ASSERT(macro2->macro.identifier != NULL);
    ASSERT(strcmp(macro2->macro.identifier, "macro2") == 0);
    ASSERT(macro2->replacement.length == 0);
    ASSERT_OK(kefir_preprocessor_user_macro_free(&kft_mem, macro2));

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(preprocessor_macro_user_scope1, "Preprocessor - user scope") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_preprocessor_user_macro_scope scope1;
    struct kefir_preprocessor_user_macro_scope scope2;
    ASSERT_OK(kefir_preprocessor_user_macro_scope_init(NULL, &scope1));
    ASSERT_OK(kefir_preprocessor_user_macro_scope_init(&scope1, &scope2));

    const struct kefir_preprocessor_user_macro *macro = NULL;
    struct kefir_preprocessor_user_macro *macro1 =
        kefir_preprocessor_user_macro_new_object(&kft_mem, &symbols, "macro1");
    struct kefir_preprocessor_user_macro *macro2 =
        kefir_preprocessor_user_macro_new_object(&kft_mem, &symbols, "macro2");
    struct kefir_preprocessor_user_macro *macro3 =
        kefir_preprocessor_user_macro_new_object(&kft_mem, &symbols, "macro3");
    struct kefir_preprocessor_user_macro *macro4 =
        kefir_preprocessor_user_macro_new_object(&kft_mem, &symbols, "macro1");
    struct kefir_preprocessor_user_macro *macro5 =
        kefir_preprocessor_user_macro_new_object(&kft_mem, &symbols, "macro3");

    ASSERT(kefir_preprocessor_user_macro_scope_at(&scope1, "macro1", &macro) == KEFIR_NOT_FOUND);
    ASSERT(kefir_preprocessor_user_macro_scope_at(&scope1, "macro2", &macro) == KEFIR_NOT_FOUND);
    ASSERT(kefir_preprocessor_user_macro_scope_at(&scope1, "macro3", &macro) == KEFIR_NOT_FOUND);
    ASSERT(kefir_preprocessor_user_macro_scope_at(&scope2, "macro1", &macro) == KEFIR_NOT_FOUND);
    ASSERT(kefir_preprocessor_user_macro_scope_at(&scope2, "macro2", &macro) == KEFIR_NOT_FOUND);
    ASSERT(kefir_preprocessor_user_macro_scope_at(&scope2, "macro3", &macro) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_preprocessor_user_macro_scope_insert(&kft_mem, &scope1, macro1));
    ASSERT_OK(kefir_preprocessor_user_macro_scope_insert(&kft_mem, &scope1, macro2));
    ASSERT_OK(kefir_preprocessor_user_macro_scope_insert(&kft_mem, &scope2, macro3));

    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope1, "macro1", &macro));
    ASSERT(macro == macro1);
    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope1, "macro2", &macro));
    ASSERT(macro == macro2);
    ASSERT(kefir_preprocessor_user_macro_scope_at(&scope1, "macro3", &macro) == KEFIR_NOT_FOUND);

    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope2, "macro1", &macro));
    ASSERT(macro == macro1);
    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope2, "macro2", &macro));
    ASSERT(macro == macro2);
    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope2, "macro3", &macro));
    ASSERT(macro == macro3);

    ASSERT_OK(kefir_preprocessor_user_macro_scope_insert(&kft_mem, &scope2, macro4));
    ASSERT_OK(kefir_preprocessor_user_macro_scope_insert(&kft_mem, &scope1, macro5));

    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope1, "macro1", &macro));
    ASSERT(macro == macro1);
    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope1, "macro2", &macro));
    ASSERT(macro == macro2);
    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope1, "macro3", &macro));
    ASSERT(macro == macro5);

    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope2, "macro1", &macro));
    ASSERT(macro == macro4);
    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope2, "macro2", &macro));
    ASSERT(macro == macro2);
    ASSERT_OK(kefir_preprocessor_user_macro_scope_at(&scope2, "macro3", &macro));
    ASSERT(macro == macro3);

    ASSERT_OK(kefir_preprocessor_user_macro_scope_free(&kft_mem, &scope2));
    ASSERT_OK(kefir_preprocessor_user_macro_scope_free(&kft_mem, &scope1));

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
