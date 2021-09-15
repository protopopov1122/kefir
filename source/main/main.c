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

static kefir_result_t open_output(const char *filepath, FILE **output) {
    if (filepath != NULL) {
        *output = fopen(filepath, "w");
        REQUIRE(*output != NULL, KEFIR_SET_OS_ERROR("Unable to open output file"));
    } else {
        *output = stdout;
    }
    return KEFIR_OK;
}

static kefir_result_t dump_action_impl(struct kefir_mem *mem, struct kefir_cli_options *options,
                                       kefir_result_t (*action)(struct kefir_mem *, struct kefir_cli_options *,
                                                                struct kefir_compiler_context *, const char *,
                                                                const char *, kefir_size_t, FILE *)) {
    FILE *output;
    struct kefir_cli_input input;
    struct kefir_compiler_profile profile;
    struct kefir_compiler_context compiler;

    const char *source_id = NULL;
    if (options->source_id != NULL) {
        source_id = options->source_id;
    } else if (options->input_filepath != NULL) {
        source_id = options->input_filepath;
    } else {
        source_id = "<stdin>";
    }

    REQUIRE_OK(open_output(options->output_filepath, &output));
    REQUIRE_OK(kefir_cli_input_open(mem, &input, options->input_filepath));
    REQUIRE_OK(kefir_compiler_profile(&profile, options->target_profile));
    REQUIRE_OK(kefir_compiler_context_init(mem, &compiler, &profile));
    REQUIRE_OK(action(mem, options, &compiler, source_id, input.content, input.length, output));
    fclose(output);
    REQUIRE_OK(kefir_compiler_context_free(mem, &compiler));
    REQUIRE_OK(kefir_cli_input_close(mem, &input));
    return KEFIR_OK;
}

static kefir_result_t dump_tokens_impl(struct kefir_mem *mem, struct kefir_cli_options *options,
                                       struct kefir_compiler_context *compiler, const char *source_id,
                                       const char *source, kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_compiler_lex(mem, compiler, &tokens, source, length, source_id));

    struct kefir_json_output json;
    REQUIRE_OK(open_output(options->output_filepath, &output));
    REQUIRE_OK(kefir_json_output_init(&json, output, 4));
    REQUIRE_OK(kefir_token_buffer_format(&json, &tokens, options->detailed_output));
    REQUIRE_OK(kefir_json_output_finalize(&json));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t action_dump_tokens(struct kefir_mem *mem, struct kefir_cli_options *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_tokens_impl));
    return KEFIR_OK;
}

static kefir_result_t dump_ast_impl(struct kefir_mem *mem, struct kefir_cli_options *options,
                                    struct kefir_compiler_context *compiler, const char *source_id, const char *source,
                                    kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    struct kefir_ast_translation_unit *unit = NULL;

    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_compiler_lex(mem, compiler, &tokens, source, length, source_id));
    REQUIRE_OK(kefir_compiler_parse(mem, compiler, &tokens, &unit));
    REQUIRE_OK(kefir_compiler_analyze(mem, compiler, KEFIR_AST_NODE_BASE(unit)));

    struct kefir_json_output json;
    REQUIRE_OK(open_output(options->output_filepath, &output));
    REQUIRE_OK(kefir_json_output_init(&json, output, 4));
    REQUIRE_OK(kefir_ast_format(&json, KEFIR_AST_NODE_BASE(unit), options->detailed_output));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t action_dump_ast(struct kefir_mem *mem, struct kefir_cli_options *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_ast_impl));
    return KEFIR_OK;
}

static kefir_result_t dump_ir_impl(struct kefir_mem *mem, struct kefir_cli_options *options,
                                   struct kefir_compiler_context *compiler, const char *source_id, const char *source,
                                   kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    struct kefir_ast_translation_unit *unit = NULL;
    struct kefir_ir_module module;

    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_compiler_lex(mem, compiler, &tokens, source, length, source_id));
    REQUIRE_OK(kefir_compiler_parse(mem, compiler, &tokens, &unit));
    REQUIRE_OK(kefir_compiler_analyze(mem, compiler, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_compiler_translate(mem, compiler, unit, &module));

    REQUIRE_OK(kefir_ir_format_module(output, &module));

    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t action_dump_ir(struct kefir_mem *mem, struct kefir_cli_options *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_ir_impl));
    return KEFIR_OK;
}

static kefir_result_t dump_asm_impl(struct kefir_mem *mem, struct kefir_cli_options *options,
                                    struct kefir_compiler_context *compiler, const char *source_id, const char *source,
                                    kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    struct kefir_ast_translation_unit *unit = NULL;
    struct kefir_ir_module module;

    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_compiler_lex(mem, compiler, &tokens, source, length, source_id));
    REQUIRE_OK(kefir_compiler_parse(mem, compiler, &tokens, &unit));
    REQUIRE_OK(kefir_compiler_analyze(mem, compiler, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_compiler_translate(mem, compiler, unit, &module));
    REQUIRE_OK(kefir_compiler_codegen(mem, compiler, &module, output));

    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(unit)));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t action_dump_asm(struct kefir_mem *mem, struct kefir_cli_options *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_asm_impl));
    return KEFIR_OK;
}

static kefir_result_t action_help(struct kefir_mem *mem, struct kefir_cli_options *options) {
    UNUSED(mem);
    UNUSED(options);
    extern const char KefirHelpContent[];
    fprintf(stdout, "%s", KefirHelpContent);
    return KEFIR_OK;
}

static kefir_result_t (*Actions[])(struct kefir_mem *, struct kefir_cli_options *) = {
    [KEFIR_CLI_ACTION_DUMP_TOKENS] = action_dump_tokens,
    [KEFIR_CLI_ACTION_DUMP_AST] = action_dump_ast,
    [KEFIR_CLI_ACTION_DUMP_IR] = action_dump_ir,
    [KEFIR_CLI_ACTION_DUMP_ASSEMBLY] = action_dump_asm,
    [KEFIR_CLI_ACTION_HELP] = action_help};

static kefir_result_t kefir_main(struct kefir_mem *mem, struct kefir_cli_options *options) {
    REQUIRE_OK(Actions[options->action](mem, options));
    return KEFIR_OK;
}

static int kefir_report_error(kefir_result_t res, struct kefir_cli_options *options) {
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
    REQUIRE_CHAIN(&res, kefir_main(kefir_system_memalloc(), &options));
    return kefir_report_error(res, &options);
}
