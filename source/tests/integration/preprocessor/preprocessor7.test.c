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
    const char CONTENT[] = "const char *a = STR();\n"
                           "const char *b = STR(2+   2\n*20);\n"
                           "const char *c = STR(STR(STR(x)));\n"
                           "const char *d = STR(\"test\\n\\0\" \"123\"\t\t\t\"\\\"\\\\\");\n"
                           "const char *e = STR(,,,,,,    1,    2);\n"
                           "const char *f = STR(\"1\"  u\"2\" \"3\" u8\"4\" \"5\" U\"6\" \"7\" L\"8\");\n"
                           "const char *g = STR(FN(2, x));";

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
    REQUIRE_OK(
        kefir_preprocessor_ast_context_init(&ast_context, &symbols, kefir_ast_default_type_traits(), &env.target_env));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_init(&virtual_source));
    REQUIRE_OK(kefir_preprocessor_context_init(&context, &virtual_source.locator, &ast_context.context));

    struct kefir_token token;
    struct kefir_preprocessor_user_macro *macro1 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "STR");
    macro1->vararg = true;
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_HASH, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro1->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "__VA_LIST__", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro1->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro1));

    struct kefir_preprocessor_user_macro *macro2 = kefir_preprocessor_user_macro_new_function(mem, &symbols, "FN");
    REQUIRE_OK(kefir_list_insert_after(mem, &macro2->parameters, kefir_list_tail(&macro2->parameters), "x"));
    REQUIRE_OK(kefir_list_insert_after(mem, &macro2->parameters, kefir_list_tail(&macro2->parameters), "y"));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "x", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_PLUS, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_LEFT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, &symbols, "y", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_RIGHT_PARENTHESE, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &macro2->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &context.user_macros, macro2));

    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &symbols, &cursor, &parser_context, &context));
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
