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
#include "kefir/core/error.h"
#include "kefir/preprocessor/virtual_source_file.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/preprocessor/format.h"
#include "kefir/preprocessor/ast_context.h"
#include "kefir/util/json.h"
#include "kefir/test/util.h"
#include <stdio.h>

static kefir_result_t ext_free(struct kefir_mem *mem, struct kefir_token *token) {
    UNUSED(mem);
    token->extension.klass = NULL;
    token->extension.payload = NULL;
    return KEFIR_OK;
}

static kefir_result_t ext_copy(struct kefir_mem *mem, struct kefir_token *dst, const struct kefir_token *src) {
    UNUSED(mem);
    REQUIRE_OK(kefir_token_new_extension(src->extension.klass, NULL, dst));
    return KEFIR_OK;
}

static kefir_result_t ext_format_json(struct kefir_json_output *json, const struct kefir_token *token,
                                      kefir_bool_t display_loc) {
    UNUSED(token);
    UNUSED(display_loc);
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    REQUIRE_OK(kefir_json_output_string(json, "at"));
    return KEFIR_OK;
}

static kefir_result_t ext_format(FILE *out, const struct kefir_token *token) {
    UNUSED(token);
    fprintf(out, "@");
    return KEFIR_OK;
}

static kefir_result_t ext_concat(struct kefir_mem *mem, const struct kefir_token *left, const struct kefir_token *right,
                                 struct kefir_token *result) {
    UNUSED(mem);
    UNUSED(left);
    UNUSED(right);
    UNUSED(result);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Concatenation is not supported");
}

static struct kefir_token_extension_class EXT_CLASS = {.free = ext_free,
                                                       .copy = ext_copy,
                                                       .concat = ext_concat,
                                                       .format = ext_format,
                                                       .format_json = ext_format_json,
                                                       .payload = NULL};

static kefir_result_t failed_lex(struct kefir_mem *mem, struct kefir_lexer *lexer, struct kefir_token *token) {
    UNUSED(mem);
    if (kefir_lexer_source_cursor_at(lexer->cursor, 0) == U'@') {
        REQUIRE_OK(kefir_token_new_extension(&EXT_CLASS, NULL, token));
        REQUIRE_OK(kefir_lexer_source_cursor_next(lexer->cursor, 1));
        return KEFIR_OK;
    }
    return KEFIR_NO_MATCH;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "int x = @+2;\n"
                           "int y[] = {[@]=_x};\n"
                           "void __fn(@);\n"
                           "#define X @(@++)*2\n"
                           "fn(X+@-000)\n"
                           "for (int @=0;){}";

    struct kefir_lexer_extensions lexer_extensions = {.failed_token_lex = failed_lex};

    struct kefir_preprocessor_extensions extensions = {.lexer_extensions = &lexer_extensions};

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
    REQUIRE_OK(kefir_preprocessor_ast_context_init(mem, &ast_context, &symbols, kefir_ast_default_type_traits(),
                                                   &env.target_env, NULL));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_init(&virtual_source));
    REQUIRE_OK(kefir_preprocessor_context_init(mem, &context, &virtual_source.locator, &ast_context.context, NULL));

    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(
        kefir_preprocessor_init(mem, &preprocessor, &symbols, &cursor, &parser_context, &context, NULL, &extensions));
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
