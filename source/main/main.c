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
#include "kefir/cli/input.h"
#include "kefir/core/util.h"
#include "kefir/compiler/compiler.h"
#include "kefir/core/os_error.h"
#include "kefir/core/error_format.h"
#include "kefir/cli/options.h"
#include "kefir/parser/format.h"
#include "kefir/ast/format.h"
#include "kefir/ir/format.h"

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

    switch (options->action) {
        case KEFIR_CLI_ACTION_DUMP_TOKENS: {
            struct kefir_json_output json;
            REQUIRE_OK(kefir_json_output_init(&json, output, 4));
            REQUIRE_OK(kefir_token_buffer_format(&json, &tokens, options->detailed_output));
            REQUIRE_OK(kefir_json_output_finalize(&json));
        } break;

        case KEFIR_CLI_ACTION_DUMP_AST: {
            struct kefir_ast_translation_unit *unit = NULL;
            REQUIRE_OK(kefir_compiler_parse(mem, &compiler, &tokens, &unit));
            REQUIRE_OK(kefir_compiler_analyze(mem, &compiler, KEFIR_AST_NODE_BASE(unit)));
            struct kefir_json_output json;
            REQUIRE_OK(kefir_json_output_init(&json, output, 4));
            REQUIRE_OK(kefir_ast_format(&json, KEFIR_AST_NODE_BASE(unit), options->detailed_output));
            REQUIRE_OK(kefir_json_output_finalize(&json));
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
        } break;

        case KEFIR_CLI_ACTION_DUMP_IR: {
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

        case KEFIR_CLI_ACTION_DUMP_ASSEMBLY: {
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

        default:
            // Intentionally left blank
            break;
    }

    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_compiler_context_free(mem, &compiler));

    if (options->output_filepath != NULL) {
        fclose(output);
    }
    return KEFIR_OK;
}

static void display_help(FILE *out) {
    extern const char KefirHelpContent[];
    fprintf(out, "%s", KefirHelpContent);
}

static kefir_result_t process(struct kefir_mem *mem, struct kefir_cli_options *options) {
    if (options->action == KEFIR_CLI_ACTION_HELP) {
        display_help(stdout);
    } else {
        struct kefir_cli_input input;
        REQUIRE_OK(kefir_cli_input_open(mem, &input, options->input_filepath));
        const char *file_content = NULL;
        kefir_size_t file_length = 0;
        REQUIRE_OK(kefir_cli_input_get(&input, &file_content, &file_length));
        REQUIRE_OK(process_code(mem, file_content, file_length, options));
        REQUIRE_OK(kefir_cli_input_close(mem, &input));
    }
    return KEFIR_OK;
}

static int report_error(kefir_result_t res, struct kefir_cli_options *options) {
    if (res == KEFIR_OK) {
        return EXIT_SUCCESS;
    } else {
        switch (options->error_report_type) {
            case KEFIR_CLI_ERROR_REPORT_TABULAR:
                fprintf(stderr, "Failed to compile! Error stack:\n");
                kefir_format_error_tabular(stderr, kefir_current_error());
                break;

            case KEFIR_CLI_ERROR_REPORT_JSON:
                kefir_format_error_json(stderr, kefir_current_error());
                break;
        }
        return EXIT_FAILURE;
    }
}

int main(int argc, char *const *argv) {
    struct kefir_cli_options options;
    kefir_result_t res = kefir_cli_parse_options(&options, argv, argc);
    REQUIRE_CHAIN(&res, process(kefir_system_memalloc(), &options));
    return report_error(res, &options);
}
