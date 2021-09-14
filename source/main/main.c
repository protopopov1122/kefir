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
#include "kefir/core/util.h"
#include "kefir/compiler/compiler.h"
#include "kefir/core/os_error.h"
#include "kefir/core/error_format.h"
#include "kefir/cli/options.h"
#include "kefir/parser/format.h"
#include "kefir/ast/format.h"
#include "kefir/ir/format.h"

static kefir_result_t mmap_file(const char *filepath, const char **content, size_t *length) {
    int fd = open(filepath, O_RDONLY);
    REQUIRE(fd >= 0, KEFIR_SET_OS_ERROR("Failed to open file"));

    struct stat statbuf;
    REQUIRE(fstat(fd, &statbuf) >= 0, KEFIR_SET_OS_ERROR("Failed to fstat file"));

    const char *ptr = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    REQUIRE(ptr != MAP_FAILED, KEFIR_SET_OS_ERROR("Failed to mmap file"));
    *content = ptr;
    *length = statbuf.st_size;
    close(fd);
    return KEFIR_OK;
}

static kefir_result_t unmap_file(const char *content, kefir_size_t length) {
    int err = munmap((void *) content, length);
    if (err != 0) {
        return KEFIR_SET_OS_ERROR("Failed to unmap file");
    }
    return KEFIR_OK;
}

static kefir_result_t read_stream(struct kefir_mem *mem, FILE *input, char **content, kefir_size_t *length) {
    *content = NULL;
    *length = 0;

    kefir_size_t file_capacity = 0;
    for (char c = fgetc(input); c != EOF; c = fgetc(input)) {
        if (*length == file_capacity) {
            file_capacity += 1024;
            *content = KEFIR_REALLOC(mem, *content, file_capacity);
            REQUIRE(*content != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to reallocate input buffer"));
        }
        (*content)[(*length)++] = c;
    }
    return KEFIR_OK;
}

static kefir_result_t process_code(struct kefir_mem *mem, const char *code, kefir_size_t length,
                                   struct kefir_cli_options *options) {
    FILE *output = stdout;
    if (options->output_filepath != NULL) {
        output = fopen(options->output_filepath, "w");
    }

    struct kefir_compiler_profile profile;
    struct kefir_compiler_context compiler;
    struct kefir_token_buffer tokens;

    REQUIRE_OK(kefir_compiler_profile(&profile, NULL));
    REQUIRE_OK(kefir_compiler_context_init(mem, &compiler, &profile));
    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_compiler_lex(mem, &compiler, &tokens, code, length,
                                  options->input_filepath != NULL ? options->input_filepath : "<stdin>"));

    switch (options->output_type) {
        case KEFIR_CLI_OUTPUT_TOKENS: {
            struct kefir_json_output json;
            REQUIRE_OK(kefir_json_output_init(&json, output, 4));
            REQUIRE_OK(kefir_token_buffer_format(&json, &tokens, options->detailed_output));
            REQUIRE_OK(kefir_json_output_finalize(&json));
        } break;

        case KEFIR_CLI_OUTPUT_AST: {
            struct kefir_ast_translation_unit *unit = NULL;
            REQUIRE_OK(kefir_compiler_parse(mem, &compiler, &tokens, &unit));
            REQUIRE_OK(kefir_compiler_analyze(mem, &compiler, KEFIR_AST_NODE_BASE(unit)));
            struct kefir_json_output json;
            REQUIRE_OK(kefir_json_output_init(&json, output, 4));
            REQUIRE_OK(kefir_ast_format(&json, KEFIR_AST_NODE_BASE(unit), options->detailed_output));
            REQUIRE_OK(kefir_json_output_finalize(&json));
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
        } break;

        case KEFIR_CLI_OUTPUT_IR: {
            struct kefir_ast_translation_unit *unit = NULL;
            struct kefir_ir_module module;
            REQUIRE_OK(kefir_compiler_parse(mem, &compiler, &tokens, &unit));
            REQUIRE_OK(kefir_compiler_analyze(mem, &compiler, KEFIR_AST_NODE_BASE(unit)));
            REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
            REQUIRE_OK(kefir_compiler_translate(mem, &compiler, unit, &module));
            REQUIRE_OK(kefir_ir_format_module(output, &module));
            REQUIRE_OK(kefir_ir_module_free(mem, &module));
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
        } break;

        case KEFIR_CLI_OUTPUT_ASSEMBLY: {
            struct kefir_ast_translation_unit *unit = NULL;
            struct kefir_ir_module module;
            REQUIRE_OK(kefir_compiler_parse(mem, &compiler, &tokens, &unit));
            REQUIRE_OK(kefir_compiler_analyze(mem, &compiler, KEFIR_AST_NODE_BASE(unit)));
            REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
            REQUIRE_OK(kefir_compiler_translate(mem, &compiler, unit, &module));
            REQUIRE_OK(kefir_compiler_codegen(mem, &compiler, &module, output));
            REQUIRE_OK(kefir_ir_module_free(mem, &module));
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
        } break;
    }

    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_compiler_context_free(mem, &compiler));

    if (options->output_filepath != NULL) {
        fclose(output);
    }
    return KEFIR_OK;
}

static kefir_result_t process(struct kefir_mem *mem, struct kefir_cli_options *options) {
    kefir_size_t file_length = 0;
    if (options->input_filepath) {
        const char *file_content = NULL;
        REQUIRE_OK(mmap_file(options->input_filepath, &file_content, &file_length));
        REQUIRE_OK(process_code(mem, file_content, file_length, options));
        REQUIRE_OK(unmap_file(file_content, file_length));
    } else {
        char *file_content = NULL;
        REQUIRE_OK(read_stream(mem, stdin, &file_content, &file_length));
        kefir_result_t res = process_code(mem, file_content, file_length, options);
        KEFIR_FREE(mem, file_content);
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

static int report_error(kefir_result_t res) {
    if (res == KEFIR_OK) {
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Failed to compile! Error stack:\n");
        kefir_format_error(stderr, kefir_current_error());
    }
    return EXIT_FAILURE;
}

static void display_help(FILE *out) {
    extern const char KefirHelpContent[];
    fprintf(out, "%s", KefirHelpContent);
}

int main(int argc, char *const *argv) {
    struct kefir_cli_options options;
    kefir_result_t res = kefir_cli_parse_options(&options, argv, argc);
    if (!options.display_help) {
        REQUIRE_CHAIN(&res, process(kefir_system_memalloc(), &options));
        return report_error(res);
    } else {
        display_help(stdout);
        return KEFIR_OK;
    }
}
