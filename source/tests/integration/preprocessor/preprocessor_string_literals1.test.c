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
#include "kefir/lexer/format.h"
#include "kefir/test/util.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "\"Hello, \" \t\t   \" cruel\\0\"    \n\" world!!!\\0\\0\\n\";\n"
                           "\"Test...\"\v\fu8\"test...\"   \t u8\"test...\\0\\\"\" \"\\0test!\\0\\n\"\n"
                           "u\"Hey!\\r\"   \t\"\\t\\n\\r\\f\\0\"    \n u\"\\0\\0\\0\\v\"u\"\\\"\\\'\"\"    \"\n"
                           "U\"\\0\\1\"   \t\v\f \"abc\"U\".!\\0\\n\"   \v\t\n   \r U\" Hello, world!\"\n"
                           "u8\"test!\\0test\", u8\"test2   \"U\"1\\0002\\0003\";\n\n\n"
                           "#define PREFIX(sth) \"This is \" #sth\n"
                           "PREFIX(2+2), u8\"\"PREFIX(100), u\"\" PREFIX(Hello) U\"\" PREFIX(\"test\")";

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

    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &symbols, &cursor, &parser_context, &context, NULL, NULL));
    REQUIRE_OK(kefir_preprocessor_run(mem, &preprocessor, &tokens));
    REQUIRE_OK(kefir_preprocessor_free(mem, &preprocessor));
    REQUIRE_OK(kefir_preprocessor_context_free(mem, &context));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_free(mem, &virtual_source));

    struct kefir_token_buffer lexer_tokens;
    REQUIRE_OK(kefir_token_buffer_init(&lexer_tokens));
    REQUIRE_OK(kefir_preprocessor_token_convert_buffer(mem, &preprocessor, &lexer_tokens, &tokens));
    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));
    REQUIRE_OK(kefir_token_buffer_format(&json, &lexer_tokens, true));
    REQUIRE_OK(kefir_json_output_finalize(&json));
    REQUIRE_OK(kefir_token_buffer_free(mem, &lexer_tokens));

    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_preprocessor_ast_context_free(mem, &ast_context));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
