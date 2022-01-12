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

#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/preprocessor/virtual_source_file.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/preprocessor/format.h"
#include "kefir/preprocessor/ast_context.h"
#include "kefir/test/util.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] =
        "int a = FN1();\n"
        "int b = FN2(200 + 100\t\t\n);\n"
        "int c = FN3(200 + 100, -1);\n"
        "int d = FN4(const long long\n\n\n\r\v, 500 - (100 + 50) /* Hello, world*/, fn1(1, 2, 3), , test);\n"
        "int e = FN5();\n"
        "int f = FN5(2+2\r*2);// Test\n"
        "int g = FN5((1, 2, 3), 4,\n(5,\n\t\t6,\n         7), ((8, 9, 10)));\n"
        "int h = FN6(test+func(1, 2, func2(3)) + 2, 1, 2, 3);\n"
        "int i = FN6(test-- * func(1, 2, func2(3)) + 2,     +100, a*b+1, fn(1, -100),,\n\n*tmp_ptr,  );\n"
        "int j = FN3(FN1(), FN2(FN1()))";

    struct kefir_symbol_table symbols;
    struct kefir_lexer_context parser_context;
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_lexer_context_default(&parser_context));
    REQUIRE_OK(kefir_token_buffer_init(&tokens));

    struct kefir_preprocessor_virtual_source_locator virtual_source;
    struct kefir_preprocessor_context context;
    struct kefir_preprocessor preprocessor;
    struct kefir_lexer_source_cursor cursor;
    struct kefir_preprocessor_ast_context ast_context;
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    REQUIRE_OK(kefir_preprocessor_ast_context_init(mem, &ast_context, &symbols, kefir_util_default_type_traits(),
                                                   &env.target_env, NULL));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_init(&virtual_source));
    REQUIRE_OK(kefir_preprocessor_context_init(mem, &context, &virtual_source.locator, &ast_context.context, NULL));

    struct kefir_token token;
    struct kefir_preprocessor_user_macro *macro1 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "FN1");
    REQUIRE_OK(kefir_token_new_pp_number(mem, "100", 3, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro1->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro1));

    struct kefir_preprocessor_user_macro *macro2 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "FN2");
    REQUIRE_OK(kefir_list_insert_after(mem, &macro2->parameters, kefir_list_tail(&macro2->parameters), "x"));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_MINUS, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "x", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro2));

    struct kefir_preprocessor_user_macro *macro3 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "FN3");
    REQUIRE_OK(kefir_list_insert_after(mem, &macro3->parameters, kefir_list_tail(&macro3->parameters), "x"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro3->parameters, kefir_list_tail(&macro3->parameters), "y"));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "x", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_STAR, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "y", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_PLUS, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "x", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro3->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro3));

    struct kefir_preprocessor_user_macro *macro4 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "FN4");
    REQUIRE_OK(kefir_list_insert_after(mem, &macro4->parameters, kefir_list_tail(&macro4->parameters), "a"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro4->parameters, kefir_list_tail(&macro4->parameters), "b"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro4->parameters, kefir_list_tail(&macro4->parameters), "c"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro4->parameters, kefir_list_tail(&macro4->parameters), "d"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro4->parameters, kefir_list_tail(&macro4->parameters), "e"));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "sizeof", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "a", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_PLUS, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "b", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_MINUS, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "c", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_SLASH, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "e", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "d", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro4->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro4));

    struct kefir_preprocessor_user_macro *macro5 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "FN5");
    macro5->vararg = true;
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "fn1", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "__VA_ARGS__", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_PLUS, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "_Alignof", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "__VA_ARGS__", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro5->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro5));

    struct kefir_preprocessor_user_macro *macro6 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "FN6");
    REQUIRE_OK(kefir_list_insert_after(mem, &macro6->parameters, kefir_list_tail(&macro6->parameters), "one"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro6->parameters, kefir_list_tail(&macro6->parameters), "two"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro6->parameters, kefir_list_tail(&macro6->parameters), "three"));
    macro6->vararg = true;
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "one", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_BRACKET, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "two", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_BRACKET, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_MINUS, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "callback", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "__VA_ARGS__", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro6->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro6));

    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &symbols, &cursor, &parser_context, &context, NULL, NULL));
    REQUIRE_OK(kefir_preprocessor_run(mem, &preprocessor, &tokens));
    REQUIRE_OK(kefir_preprocessor_free(mem, &preprocessor));
    REQUIRE_OK(kefir_preprocessor_context_free(mem, &context));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_free(mem, &virtual_source));

    REQUIRE_OK(kefir_preprocessor_format(stdout, &tokens, KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_ORIGINAL));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_preprocessor_ast_context_free(mem, &ast_context));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
