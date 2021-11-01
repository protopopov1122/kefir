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

struct extension_payload {
    struct kefir_preprocessor_user_macro_scope scope;
    struct kefir_preprocessor_overlay_macro_scope overlay;
};

static kefir_result_t on_init(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor) {
    struct extension_payload *payload = KEFIR_MALLOC(mem, sizeof(struct extension_payload));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_init(NULL, &payload->scope));
    REQUIRE_OK(
        kefir_preprocessor_overlay_macro_scope_init(&payload->overlay, &payload->scope.scope, preprocessor->macros));
    preprocessor->macros = &payload->overlay.scope;
    preprocessor->extension_payload = payload;

    struct kefir_preprocessor_user_macro *user_macro =
        kefir_preprocessor_user_macro_new_object(mem, preprocessor->lexer.symbols, "HAS_TOTAL_LINES");
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_number(mem, "1", 1, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, &user_macro->replacement, &token));
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_insert(mem, &payload->scope, user_macro));
    return KEFIR_OK;
}

static kefir_result_t on_free(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor) {
    struct extension_payload *payload = (struct extension_payload *) preprocessor->extension_payload;
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_free(mem, &payload->scope));
    KEFIR_FREE(mem, payload);
    preprocessor->extension_payload = NULL;
    preprocessor->macros = &preprocessor->macro_overlay.scope;
    return KEFIR_OK;
}

static kefir_result_t before_run(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                 struct kefir_token_buffer *buffer) {
    if (preprocessor->parent != NULL) {
        return KEFIR_OK;
    }
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "static", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(false, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "const", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(false, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "unsigned", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(false, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "TOTAL_LINES", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_SEMICOLON, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(true, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(true, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    return KEFIR_OK;
}

static kefir_result_t after_run(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                struct kefir_token_buffer *buffer) {
    if (preprocessor->parent != NULL) {
        return KEFIR_OK;
    }
    char BUF[256] = {0};
    int buflen = snprintf(BUF, sizeof(BUF) - 1, "%u", preprocessor->lexer.cursor->location.line);

    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_pp_whitespace(true, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(true, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "static", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(false, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "const", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(false, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "unsigned", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_whitespace(false, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_identifier(mem, preprocessor->lexer.symbols, "TOTAL_LINES", &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_ASSIGN, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_pp_number(mem, BUF, buflen, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    REQUIRE_OK(kefir_token_new_punctuator(KEFIR_PUNCTUATOR_SEMICOLON, &token));
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    return KEFIR_OK;
}

static kefir_result_t on_directive(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                   struct kefir_preprocessor_directive *directive) {
    UNUSED(preprocessor);
    if (directive->type == KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE &&
        strcmp(directive->define_directive.identifier, "HAS_TOTAL_LINES") == 0) {
        REQUIRE_OK(kefir_preprocessor_directive_free(mem, directive));
        directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY;
    }
    return KEFIR_OK;
}

struct context_payload {
    struct kefir_preprocessor_source_locator locator;
};

static kefir_result_t close_source(struct kefir_mem *mem, struct kefir_preprocessor_source_file *source_file) {
    UNUSED(mem);
    *source_file = (struct kefir_preprocessor_source_file){0};
    return KEFIR_OK;
}

static kefir_result_t open_source(struct kefir_mem *mem, const struct kefir_preprocessor_source_locator *source_locator,
                                  const char *filepath, kefir_bool_t system, const char *current_filepath,
                                  struct kefir_preprocessor_source_file *source_file) {
    static const char CONTENT[] = "void some_function();\n";
    if (strcmp(filepath, "some_include") == 0) {
        source_file->filepath = filepath;
        source_file->system = system;
        source_file->payload = NULL;
        source_file->close = close_source;
        REQUIRE_OK(kefir_lexer_source_cursor_init(&source_file->cursor, CONTENT, sizeof(CONTENT), filepath));
    } else {
        struct kefir_preprocessor_source_locator *original = source_locator->payload;
        REQUIRE_OK(original->open(mem, original, filepath, system, current_filepath, source_file));
    }
    return KEFIR_OK;
}

static kefir_result_t on_context_init(struct kefir_mem *mem, struct kefir_preprocessor_context *context) {
    struct context_payload *payload = KEFIR_MALLOC(mem, sizeof(struct context_payload));
    payload->locator.payload = (void *) context->source_locator;
    payload->locator.open = open_source;
    context->source_locator = &payload->locator;
    context->extensions_payload = payload;
    return KEFIR_OK;
}

static kefir_result_t on_context_free(struct kefir_mem *mem, struct kefir_preprocessor_context *context) {
    struct context_payload *payload = context->extensions_payload;
    context->source_locator = payload->locator.payload;
    KEFIR_FREE(mem, payload);
    context->extensions_payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "#define HAS_TOTAL_LINES 0\n"
                           "#define SOME_OTHER_MACRO 100\n"
                           "#if HAS_TOTAL_LINES\n"
                           "int get_total_lines() {\n"
                           "    return total_lines + SOME_OTHER_MACRO;\n"
                           "}\n"
                           "#include <some_include>\n"
                           "#endif";

    struct kefir_preprocessor_extensions extensions = {.on_init = on_init,
                                                       .on_free = on_free,
                                                       .before_run = before_run,
                                                       .after_run = after_run,
                                                       .on_next_directive = on_directive};

    struct kefir_preprocessor_context_extensions context_extensions = {.on_init = on_context_init,
                                                                       .on_free = on_context_free};

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
    REQUIRE_OK(kefir_preprocessor_context_init(mem, &context, &virtual_source.locator, &ast_context.context,
                                               &context_extensions));

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
