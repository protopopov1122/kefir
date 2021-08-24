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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/parser/parser.h"
#include "kefir/parser/rules.h"
#include "kefir/parser/lexer.h"

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

static kefir_result_t mmap_file(const char *filepath, const char **content, size_t *length) {
    int fd = open(filepath, O_RDONLY);
    REQUIRE(fd >= 0, KEFIR_SET_ERROR(KEFIR_OS_ERROR, "Failed to open file"));

    struct stat statbuf;
    REQUIRE(fstat(fd, &statbuf) >= 0, KEFIR_SET_ERROR(KEFIR_OS_ERROR, "Failed to fstat file"));

    const char *ptr = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    REQUIRE(ptr != MAP_FAILED, KEFIR_SET_ERROR(KEFIR_OS_ERROR, "Failed to mmap file"));
    *content = ptr;
    *length = statbuf.st_size;
    close(fd);
    return KEFIR_OK;
}

static kefir_result_t unmap_file(const char *content, kefir_size_t length) {
    int err = munmap((void *) content, length);
    if (err != 0) {
        return KEFIR_SET_ERROR(KEFIR_OS_ERROR, "Failed to unmap file");
    }
    return KEFIR_OK;
}

static kefir_result_t parse_unit(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                 struct kefir_ast_translation_unit **result, const char *code,
                                 kefir_size_t code_length) {
    struct kefir_lexer_source_cursor source_cursor;
    struct kefir_parser_integral_types integral_types;
    struct kefir_lexer lexer;
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_lexer_source_cursor_init(&source_cursor, code, code_length));
    REQUIRE_OK(kefir_parser_integral_types_default(&integral_types));
    REQUIRE_OK(kefir_lexer_init(mem, &lexer, context->symbols, &source_cursor, &integral_types));
    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_token_buffer_consume(mem, &tokens, &lexer));
    REQUIRE_OK(kefir_lexer_free(mem, &lexer));

    struct kefir_parser_token_cursor cursor;
    struct kefir_parser parser;

    REQUIRE_OK(kefir_parser_token_cursor_init(&cursor, tokens.tokens, tokens.length));
    REQUIRE_OK(kefir_parser_init(mem, &parser, context->symbols, &cursor));
    struct kefir_ast_node_base *node = NULL;
    REQUIRE_OK(KEFIR_PARSER_NEXT_TRANSLATION_UNIT(mem, &parser, &node));
    *result = node->self;

    REQUIRE_OK(kefir_parser_free(mem, &parser));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform, const char *code,
                                  kefir_size_t code_length) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));

    struct kefir_ast_translation_unit *unit = NULL;
    REQUIRE_OK(parse_unit(mem, &global_context.context, &unit, code, code_length));

    REQUIRE_OK(kefir_ast_analyze_node(mem, &global_context.context, KEFIR_AST_NODE_BASE(unit)));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    translator_context.global_scope_layout = &global_scope;

    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));
    REQUIRE_OK(kefir_ast_translate_unit(mem, KEFIR_AST_NODE_BASE(unit), &translator_context));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

static kefir_result_t compile_code(const char *code, kefir_size_t length) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_target_platform ir_target;
    struct kefir_ir_module module;
    struct kefir_mem *mem = &MemoryAllocator;

    REQUIRE_OK(kefir_codegen_amd64_sysv_target_platform(&ir_target));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(generate_ir(mem, &module, &ir_target, code, length));
    KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module);
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    KEFIR_CODEGEN_CLOSE(&codegen.iface);
    return KEFIR_OK;
}

static kefir_result_t compiler(const char *filepath) {
    const char *file_content = NULL;
    kefir_size_t file_length = 0;
    REQUIRE_OK(mmap_file(filepath, &file_content, &file_length));
    REQUIRE_OK(compile_code(file_content, file_length));
    REQUIRE_OK(unmap_file(file_content, file_length));
    return KEFIR_OK;
}

static int report_error(kefir_result_t res) {
    if (res == KEFIR_OK) {
        return EXIT_SUCCESS;
    } else if (res == KEFIR_OS_ERROR) {
        fprintf(stderr, "Failed to compile! Error stack:\n");
        const struct kefir_error *err = kefir_current_error();
        fprintf(stderr, "\t%s:%u\t", err->file, err->line);
        perror(kefir_current_error()->message);
    } else {
        fprintf(stderr, "Failed to compile! Error stack:\n");
        const struct kefir_error *err = kefir_current_error();
        if (err->error_overflow) {
            fprintf(stderr, "[Omitted due to overflow]\n");
        }
        while (err != NULL) {
            fprintf(stderr, "\t%s:%u\t%s\n", err->file, err->line, err->message);
            err = err->prev_error;
        }
    }
    return EXIT_FAILURE;
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        printf("Usage: %s file-name", argv[0]);
        return EXIT_FAILURE;
    }
    kefir_result_t res = compiler(argv[1]);
    return report_error(res);
}
