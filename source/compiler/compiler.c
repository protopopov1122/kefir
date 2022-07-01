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

#include "kefir/compiler/compiler.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/parser/parser.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/preprocessor/tokenizer.h"
#include "kefir/ast/downcast.h"

static void *kefir_malloc(struct kefir_mem *mem, kefir_size_t sz) {
    UNUSED(mem);
    return malloc(sz);
}

static void *kefir_calloc(struct kefir_mem *mem, kefir_size_t num, kefir_size_t sz) {
    UNUSED(mem);
    return calloc(num, sz);
}

static void *kefir_realloc(struct kefir_mem *mem, void *ptr, kefir_size_t sz) {
    UNUSED(mem);
    return realloc(ptr, sz);
}

static void kefir_free(struct kefir_mem *mem, void *ptr) {
    UNUSED(mem);
    free(ptr);
}

static struct kefir_mem MemoryAllocator = {
    .malloc = kefir_malloc, .calloc = kefir_calloc, .realloc = kefir_realloc, .free = kefir_free, .data = NULL};

struct kefir_mem *kefir_system_memalloc() {
    return &MemoryAllocator;
}

kefir_result_t kefir_compiler_context_init(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                           struct kefir_compiler_profile *profile,
                                           const struct kefir_preprocessor_source_locator *source_locator,
                                           const struct kefir_compiler_extensions *extensions) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(profile != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler profile"));
    REQUIRE(source_locator != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler source locator"));

    REQUIRE_OK(kefir_parser_configuration_default(&context->parser_configuration));
    REQUIRE_OK(kefir_ast_translator_configuration_default(&context->translator_configuration));
    REQUIRE_OK(kefir_ast_translator_environment_init(&context->translator_env, &profile->ir_target_platform));
    context->translator_env.configuration = &context->translator_configuration;
    REQUIRE_OK(kefir_ast_global_context_init(mem, profile->type_traits, &context->translator_env.target_env,
                                             &context->ast_global_context,
                                             extensions != NULL ? extensions->analyzer : NULL));
    kefir_result_t res = kefir_preprocessor_ast_context_init(
        mem, &context->preprocessor_ast_context, &context->ast_global_context.symbols, profile->type_traits,
        &context->translator_env.target_env, extensions != NULL ? extensions->analyzer : NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_global_context_free(mem, &context->ast_global_context);
        return res;
    });
    res = kefir_preprocessor_context_init(mem, &context->preprocessor_context, source_locator,
                                          &context->preprocessor_ast_context.context,
                                          extensions != NULL ? extensions->preprocessor_context : NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_ast_context_free(mem, &context->preprocessor_ast_context);
        kefir_ast_global_context_free(mem, &context->ast_global_context);
        return res;
    });
    context->codegen_configuration = KefirCodegenDefaultConfiguration;
    context->preprocessor_context.environment.data_model = profile->data_model;
    context->profile = profile;
    context->source_locator = source_locator;

    context->extensions = extensions;
    context->extension_payload = NULL;
    if (context->extensions != NULL && context->extensions->on_init != NULL) {
        res = context->extensions->on_init(mem, context);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_preprocessor_context_free(mem, &context->preprocessor_context);
            kefir_preprocessor_ast_context_free(mem, &context->preprocessor_ast_context);
            kefir_ast_global_context_free(mem, &context->ast_global_context);
            return res;
        });
    }
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_context_free(struct kefir_mem *mem, struct kefir_compiler_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));

    if (context->extensions != NULL && context->extensions->on_free != NULL) {
        REQUIRE_OK(context->extensions->on_free(mem, context));
        context->extensions = NULL;
        context->extension_payload = NULL;
    }

    REQUIRE_OK(kefir_preprocessor_context_free(mem, &context->preprocessor_context));
    REQUIRE_OK(kefir_preprocessor_ast_context_free(mem, &context->preprocessor_ast_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &context->ast_global_context));
    context->profile = NULL;
    context->source_locator = NULL;
    return KEFIR_OK;
}

static kefir_result_t preprocessor_tokenize_impl(struct kefir_mem *mem, struct kefir_token_buffer *buffer,
                                                 struct kefir_lexer *lexer) {
    struct kefir_token token;
    kefir_bool_t skip_whitespaces = true;
    while (skip_whitespaces) {
        REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, lexer, &token));
        if (token.klass != KEFIR_TOKEN_PP_WHITESPACE || token.pp_whitespace.newline) {
            skip_whitespaces = false;
        } else {
            REQUIRE_OK(kefir_token_free(mem, &token));
        }
    }

    while (token.klass != KEFIR_TOKEN_SENTINEL) {
        REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
        REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, lexer, &token));
    }
    REQUIRE_OK(kefir_token_free(mem, &token));

    while (buffer->length > 0 && buffer->tokens[buffer->length - 1].klass == KEFIR_TOKEN_PP_WHITESPACE) {
        REQUIRE_OK(kefir_token_buffer_pop(mem, buffer));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_preprocessor_tokenize(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                                    struct kefir_token_buffer *buffer, const char *content,
                                                    kefir_size_t length, const char *source_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid content"));

    struct kefir_lexer_source_cursor source_cursor;
    struct kefir_lexer lexer;
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_cursor, content, length, source_id));
    REQUIRE_OK(kefir_lexer_init(mem, &lexer, &context->ast_global_context.symbols, &source_cursor,
                                &context->profile->lexer_context,
                                context->extensions != NULL ? context->extensions->lexer : NULL));

    kefir_result_t res = preprocessor_tokenize_impl(mem, buffer, &lexer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_lexer_free(mem, &lexer);
        return res;
    });
    REQUIRE_OK(kefir_lexer_free(mem, &lexer));
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_preprocess(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                         struct kefir_token_buffer *buffer, const char *content, kefir_size_t length,
                                         const char *source_id, const char *filepath) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid content"));

    struct kefir_lexer_source_cursor source_cursor;
    struct kefir_preprocessor preprocessor;
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_cursor, content, length, source_id));
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &context->ast_global_context.symbols, &source_cursor,
                                       &context->profile->lexer_context, &context->preprocessor_context, filepath,
                                       context->extensions != NULL ? context->extensions->preprocessor : NULL));

    kefir_result_t res = kefir_preprocessor_run(mem, &preprocessor, buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_free(mem, &preprocessor);
        return res;
    });
    REQUIRE_OK(kefir_preprocessor_free(mem, &preprocessor));
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_preprocess_lex(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                             struct kefir_token_buffer *buffer, const char *content,
                                             kefir_size_t length, const char *source_id, const char *filepath) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid content"));

    struct kefir_lexer_source_cursor source_cursor;
    struct kefir_preprocessor preprocessor;
    struct kefir_token_buffer pp_tokens;
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_cursor, content, length, source_id));
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &context->ast_global_context.symbols, &source_cursor,
                                       &context->profile->lexer_context, &context->preprocessor_context, filepath,
                                       context->extensions != NULL ? context->extensions->preprocessor : NULL));
    kefir_result_t res = kefir_token_buffer_init(&pp_tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_free(mem, &preprocessor);
        return res;
    });

    res = kefir_preprocessor_run(mem, &preprocessor, &pp_tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &pp_tokens);
        kefir_preprocessor_free(mem, &preprocessor);
        return res;
    });

    res = kefir_preprocessor_token_convert_buffer(mem, &preprocessor, buffer, &pp_tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &pp_tokens);
        kefir_preprocessor_free(mem, &preprocessor);
        return res;
    });

    res = kefir_token_buffer_free(mem, &pp_tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_free(mem, &preprocessor);
        return res;
    });

    REQUIRE_OK(kefir_preprocessor_free(mem, &preprocessor));
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_preprocess_include(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                                 struct kefir_token_buffer *buffer, const char *source_id,
                                                 const char *filepath) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(filepath != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid include file path"));

    kefir_result_t res;
    struct kefir_preprocessor_source_file source_file;

    REQUIRE_OK(context->preprocessor_context.source_locator->open(mem, context->preprocessor_context.source_locator,
                                                                  filepath, false, NULL, &source_file));

    res = kefir_compiler_preprocess_lex(mem, context, buffer, source_file.cursor.content, source_file.cursor.length,
                                        source_id, filepath);
    REQUIRE_ELSE(res == KEFIR_OK, {
        source_file.close(mem, &source_file);
        return res;
    });
    REQUIRE_OK(source_file.close(mem, &source_file));

    if (buffer->length > 0 && buffer->tokens[buffer->length - 1].klass == KEFIR_TOKEN_SENTINEL) {
        REQUIRE_OK(kefir_token_buffer_pop(mem, buffer));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_lex(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                  struct kefir_token_buffer *buffer, const char *content, kefir_size_t length,
                                  const char *source_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid content"));

    struct kefir_lexer_source_cursor source_cursor;
    struct kefir_lexer lexer;
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_cursor, content, length, source_id));
    REQUIRE_OK(kefir_lexer_init(mem, &lexer, &context->ast_global_context.symbols, &source_cursor,
                                &context->profile->lexer_context,
                                context->extensions != NULL ? context->extensions->lexer : NULL));
    kefir_result_t res = kefir_lexer_populate_buffer(mem, buffer, &lexer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_lexer_free(mem, &lexer);
        return res;
    });
    REQUIRE_OK(kefir_lexer_free(mem, &lexer));
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_parse(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                    struct kefir_token_buffer *buffer, struct kefir_ast_translation_unit **unit_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    REQUIRE(unit_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer AST translation unit"));

    struct kefir_parser_token_cursor cursor;
    struct kefir_parser parser;

    REQUIRE_OK(kefir_parser_token_cursor_init(&cursor, buffer->tokens, buffer->length));
    REQUIRE_OK(kefir_parser_init(mem, &parser, &context->ast_global_context.symbols, &cursor,
                                 context->extensions != NULL ? context->extensions->parser : NULL));
    parser.configuration = &context->parser_configuration;
    struct kefir_ast_node_base *node = NULL;
    kefir_result_t res = KEFIR_PARSER_NEXT_TRANSLATION_UNIT(mem, &parser, &node);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_parser_free(mem, &parser);
        return res;
    });

    REQUIRE_MATCH(&res, kefir_ast_downcast_translation_unit(node, unit_ptr, true),
                  KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected parser to produce AST translation unit"));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, node);
        kefir_parser_free(mem, &parser);
        return res;
    });

    res = kefir_parser_free(mem, &parser);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, node);
        *unit_ptr = NULL;
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_parse_source(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                           const char *content, kefir_size_t length, const char *source_id,
                                           struct kefir_ast_translation_unit **unit_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(content != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid content"));
    REQUIRE(source_id != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid source identifier"));
    REQUIRE(unit_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer AST translation unit"));

    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    kefir_result_t res = kefir_compiler_lex(mem, context, &tokens, content, length, source_id);
    REQUIRE_CHAIN(&res, kefir_compiler_parse(mem, context, &tokens, unit_ptr));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &tokens);
        return res;
    });

    res = kefir_token_buffer_free(mem, &tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(*unit_ptr));
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_analyze(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                      struct kefir_ast_node_base *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE_OK(kefir_ast_analyze_node(mem, &context->ast_global_context.context, node));
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_translate(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                        struct kefir_ast_translation_unit *unit, struct kefir_ir_module *module) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(unit != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translation unit"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(mem, &translator_context, &context->ast_global_context.context,
                                                 &context->translator_env, module,
                                                 context->extensions != NULL ? context->extensions->translator : NULL));

    struct kefir_ast_translator_global_scope_layout global_scope;
    kefir_result_t res = kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &translator_context);
        return res;
    });
    translator_context.global_scope_layout = &global_scope;

    res = kefir_ast_translator_build_global_scope_layout(mem, module, &context->ast_global_context,
                                                         translator_context.environment, &global_scope);
    REQUIRE_CHAIN(&res, kefir_ast_translate_unit(mem, KEFIR_AST_NODE_BASE(unit), &translator_context));
    REQUIRE_CHAIN(&res,
                  kefir_ast_translate_global_scope(mem, &context->ast_global_context.context, module, &global_scope));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_global_scope_layout_free(mem, &global_scope);
        kefir_ast_translator_context_free(mem, &translator_context);
        return res;
    });

    res = kefir_ast_translator_global_scope_layout_free(mem, &global_scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &translator_context);
        return res;
    });
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_codegen(struct kefir_mem *mem, struct kefir_compiler_context *context,
                                      struct kefir_ir_module *module, FILE *output) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid compiler context"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(output != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid FILE"));

    struct kefir_codegen *codegen = NULL;
    REQUIRE_OK(context->profile->new_codegen(mem, output, &context->codegen_configuration, &codegen));
    kefir_result_t res = KEFIR_CODEGEN_TRANSLATE(mem, codegen, module);
    REQUIRE_ELSE(res == KEFIR_OK, {
        context->profile->free_codegen(mem, codegen);
        return res;
    });
    REQUIRE_OK(context->profile->free_codegen(mem, codegen));
    return KEFIR_OK;
}
