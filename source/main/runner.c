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
#include <locale.h>
#include "kefir/cli/input.h"
#include "kefir/cli/filesystem_source.h"
#include "kefir/core/util.h"
#include "kefir/compiler/compiler.h"
#include "kefir/core/os_error.h"
#include "kefir/core/error_format.h"
#include "kefir/cli/options.h"
#include "kefir/lexer/format.h"
#include "kefir/ast/format.h"
#include "kefir/ir/format.h"
#include "kefir/preprocessor/format.h"
#include "kefir/core/version.h"
#include "kefir/main/runner.h"

// ATTENTION: This is module is not a part of the core library, thus memory management
//            is different here. While all the modules from core library shall correctly
//            handle memory deallocations in all cases (leaks, use-after-free, double free,
//            etc. are considered bugs), in standalone application part this rule is relaxed.
//            Specifically, correct memory deallocation is not deemed necessary, as it is
//            known that all the memory will be eventually deallocated by the OS. At the same time,
//            it is beneficially to correctly deallocate memory when there are no runtime errors:
//            it enables Valgrind use in end2end tests, thus increasing dynamic analysis coverage.
//            Based on this idea, code below is written with following assumptions:
//                - In case of happy-path, memory deallocations should happen correctly with no
//                  Valgrind warnings.
//                - In case of runtime errors, memory deallocations might be omitted. Valgrind
//                  warnings are considered normal.
//                - Other memory management issues (use-after-frees, double frees, etc.) are
//                  considered unacceptable and should be fixed.

static kefir_result_t open_output(const char *filepath, FILE **output) {
    if (filepath != NULL) {
        *output = fopen(filepath, "w");
        REQUIRE(*output != NULL, KEFIR_SET_OS_ERROR("Unable to open output file"));
    } else {
        *output = stdout;
    }
    return KEFIR_OK;
}

static kefir_result_t dump_action_impl(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                                       kefir_result_t (*action)(struct kefir_mem *,
                                                                struct kefir_compiler_runner_configuration *,
                                                                struct kefir_compiler_context *, const char *,
                                                                const char *, kefir_size_t, FILE *)) {
    FILE *output;
    struct kefir_cli_input input;
    struct kefir_compiler_profile profile;
    struct kefir_compiler_context compiler;
    struct kefir_preprocessor_filesystem_source_locator source_locator;

    const char *source_id = NULL;
    if (options->source_id != NULL) {
        source_id = options->source_id;
    } else if (options->input_filepath != NULL) {
        source_id = options->input_filepath;
    } else {
        source_id = "<stdin>";
    }

    REQUIRE_OK(open_output(options->output_filepath, &output));
    REQUIRE_OK(kefir_cli_input_open(mem, &input, options->input_filepath, stdin));
    REQUIRE_OK(
        kefir_preprocessor_filesystem_source_locator_init(&source_locator, &compiler.ast_global_context.symbols));
    for (const struct kefir_list_entry *iter = kefir_list_head(&options->include_path); iter != NULL;
         kefir_list_next(&iter)) {
        REQUIRE_OK(
            kefir_preprocessor_filesystem_source_locator_append(mem, &source_locator, (const char *) iter->value));
    }
    REQUIRE_OK(kefir_compiler_profile(&profile, options->target_profile));
    REQUIRE_OK(kefir_compiler_context_init(mem, &compiler, &profile, &source_locator.locator, NULL));

    compiler.parser_configuration.fail_on_attributes = options->features.fail_on_attributes;
    compiler.parser_configuration.implicit_function_definition_int = options->features.missing_function_return_type;
    compiler.parser_configuration.designated_initializer_colons = options->features.designated_initializer_colons;
    compiler.parser_configuration.label_addressing = options->features.labels_as_values;
    compiler.parser_configuration.statement_expressions = options->features.statement_expressions;
    compiler.parser_configuration.omitted_conditional_operand = options->features.omitted_conditional_operand;

    compiler.ast_global_context.configuration.analysis.non_strict_qualifiers = options->features.non_strict_qualifiers;
    compiler.ast_global_context.configuration.analysis.fixed_enum_type = options->features.signed_enum_type;
    compiler.ast_global_context.configuration.analysis.implicit_function_declaration =
        options->features.implicit_function_declaration;
    compiler.ast_global_context.configuration.analysis.ext_pointer_arithmetics =
        options->features.ext_pointer_arithmetics;
    compiler.ast_global_context.configuration.analysis.missing_braces_subobj =
        options->features.missing_braces_subobject;
    compiler.ast_global_context.configuration.analysis.int_to_pointer = options->features.int_to_pointer;
    compiler.ast_global_context.configuration.analysis.permissive_pointer_conv =
        options->features.permissive_pointer_conv;

    compiler.translator_configuration.empty_structs = options->features.empty_structs;

    compiler.codegen_configuration.emulated_tls = options->codegen.emulated_tls;

    REQUIRE_OK(action(mem, options, &compiler, source_id, input.content, input.length, output));
    fclose(output);
    REQUIRE_OK(kefir_compiler_context_free(mem, &compiler));
    REQUIRE_OK(kefir_preprocessor_filesystem_source_locator_free(mem, &source_locator));
    REQUIRE_OK(kefir_cli_input_close(mem, &input));
    return KEFIR_OK;
}

static kefir_result_t build_predefined_macros(struct kefir_mem *mem,
                                              struct kefir_compiler_runner_configuration *options,
                                              struct kefir_compiler_context *compiler) {
    struct kefir_hashtree_node_iterator macro_iter;
    const struct kefir_hashtree_node *macro_node = kefir_hashtree_iter(&options->defines, &macro_iter);
    for (; macro_node != NULL; macro_node = kefir_hashtree_next(&macro_iter)) {
        ASSIGN_DECL_CAST(const char *, identifier, macro_node->key);
        ASSIGN_DECL_CAST(const char *, raw_value, macro_node->value);

        struct kefir_preprocessor_user_macro *macro =
            kefir_preprocessor_user_macro_new_object(mem, &compiler->ast_global_context.symbols, identifier);
        REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate preprocessor macro"));
        kefir_result_t res = KEFIR_OK;

        if (raw_value != NULL) {
            res = kefir_compiler_preprocessor_tokenize(mem, compiler, &macro->replacement, raw_value, strlen(raw_value),
                                                       identifier);
        }
        REQUIRE_CHAIN(
            &res, kefir_preprocessor_user_macro_scope_insert(mem, &compiler->preprocessor_context.user_macros, macro));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_preprocessor_user_macro_free(mem, macro);
            return res;
        });
    }

    return KEFIR_OK;
}

static kefir_result_t include_predefined(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                                         struct kefir_compiler_context *compiler, const char *source_id,
                                         struct kefir_token_buffer *tokens) {
    for (const struct kefir_list_entry *iter = kefir_list_head(&options->include_files); iter != NULL;
         kefir_list_next(&iter)) {
        REQUIRE_OK(kefir_compiler_preprocess_include(mem, compiler, tokens, source_id, (const char *) iter->value));
    }
    return KEFIR_OK;
}

static kefir_result_t lex_file(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                               struct kefir_compiler_context *compiler, const char *source_id, const char *source,
                               kefir_size_t length, struct kefir_token_buffer *tokens) {
    if (options->skip_preprocessor) {
        REQUIRE_OK(kefir_compiler_lex(mem, compiler, tokens, source, length, source_id));
    } else {
        REQUIRE_OK(build_predefined_macros(mem, options, compiler));
        if (!options->default_pp_timestamp) {
            compiler->preprocessor_context.environment.timestamp = options->pp_timestamp;
        }
        REQUIRE_OK(include_predefined(mem, options, compiler, source_id, tokens));
        REQUIRE_OK(
            kefir_compiler_preprocess_lex(mem, compiler, tokens, source, length, source_id, options->input_filepath));
    }
    return KEFIR_OK;
}

static kefir_result_t dump_preprocessed_impl(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                                             struct kefir_compiler_context *compiler, const char *source_id,
                                             const char *source, kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    REQUIRE_OK(build_predefined_macros(mem, options, compiler));
    REQUIRE_OK(include_predefined(mem, options, compiler, source_id, &tokens));
    REQUIRE_OK(kefir_compiler_preprocess(mem, compiler, &tokens, source, length, source_id, options->input_filepath));
    REQUIRE_OK(open_output(options->output_filepath, &output));
    REQUIRE_OK(kefir_preprocessor_format(output, &tokens, KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_ORIGINAL));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t action_dump_preprocessed(struct kefir_mem *mem,
                                               struct kefir_compiler_runner_configuration *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_preprocessed_impl));
    return KEFIR_OK;
}

static kefir_result_t dump_tokens_impl(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                                       struct kefir_compiler_context *compiler, const char *source_id,
                                       const char *source, kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    REQUIRE_OK(lex_file(mem, options, compiler, source_id, source, length, &tokens));

    struct kefir_json_output json;
    REQUIRE_OK(open_output(options->output_filepath, &output));
    REQUIRE_OK(kefir_json_output_init(&json, output, 4));
    REQUIRE_OK(kefir_token_buffer_format(&json, &tokens, options->detailed_output));
    REQUIRE_OK(kefir_json_output_finalize(&json));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    return KEFIR_OK;
}

static kefir_result_t action_dump_tokens(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_tokens_impl));
    return KEFIR_OK;
}

static kefir_result_t dump_ast_impl(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                                    struct kefir_compiler_context *compiler, const char *source_id, const char *source,
                                    kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    struct kefir_ast_translation_unit *unit = NULL;

    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    REQUIRE_OK(lex_file(mem, options, compiler, source_id, source, length, &tokens));
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

static kefir_result_t action_dump_ast(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_ast_impl));
    return KEFIR_OK;
}

static kefir_result_t dump_ir_impl(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                                   struct kefir_compiler_context *compiler, const char *source_id, const char *source,
                                   kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    struct kefir_ast_translation_unit *unit = NULL;
    struct kefir_ir_module module;

    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    REQUIRE_OK(lex_file(mem, options, compiler, source_id, source, length, &tokens));
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

static kefir_result_t action_dump_ir(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_ir_impl));
    return KEFIR_OK;
}

static kefir_result_t dump_asm_impl(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options,
                                    struct kefir_compiler_context *compiler, const char *source_id, const char *source,
                                    kefir_size_t length, FILE *output) {
    UNUSED(options);
    struct kefir_token_buffer tokens;
    struct kefir_ast_translation_unit *unit = NULL;
    struct kefir_ir_module module;

    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    REQUIRE_OK(lex_file(mem, options, compiler, source_id, source, length, &tokens));
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

static kefir_result_t action_dump_asm(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options) {
    REQUIRE_OK(dump_action_impl(mem, options, dump_asm_impl));
    return KEFIR_OK;
}

static kefir_result_t action_dump_runtime_code(struct kefir_mem *mem,
                                               struct kefir_compiler_runner_configuration *options) {
    UNUSED(mem);
    struct kefir_compiler_profile profile;
    REQUIRE_OK(kefir_compiler_profile(&profile, options->target_profile));
    REQUIRE(profile.runtime_code != NULL,
            KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Runtime code is not available for specified profile"));
    printf("%s", profile.runtime_code);
    return KEFIR_OK;
}

static kefir_result_t action_help(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options) {
    UNUSED(mem);
    UNUSED(options);
    extern const char KefirHelpContent[];
    printf("%s", KefirHelpContent);
    return KEFIR_OK;
}

static kefir_result_t action_version(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options) {
    UNUSED(mem);
    UNUSED(options);
    printf("%u.%u.%u\n", KEFIR_VERSION_MAJOR, KEFIR_VERSION_MINOR, KEFIR_VERSION_PATCH);
    return KEFIR_OK;
}

static kefir_result_t (*Actions[])(struct kefir_mem *, struct kefir_compiler_runner_configuration *) = {
    [KEFIR_COMPILER_RUNNER_ACTION_PREPROCESS] = action_dump_preprocessed,
    [KEFIR_COMPILER_RUNNER_ACTION_DUMP_TOKENS] = action_dump_tokens,
    [KEFIR_COMPILER_RUNNER_ACTION_DUMP_AST] = action_dump_ast,
    [KEFIR_COMPILER_RUNNER_ACTION_DUMP_IR] = action_dump_ir,
    [KEFIR_COMPILER_RUNNER_ACTION_DUMP_ASSEMBLY] = action_dump_asm,
    [KEFIR_COMPILER_RUNNER_ACTION_DUMP_RUNTIME_CODE] = action_dump_runtime_code,
    [KEFIR_COMPILER_RUNNER_ACTION_HELP] = action_help,
    [KEFIR_COMPILER_RUNNER_ACTION_VERSION] = action_version};

kefir_result_t kefir_run_compiler(struct kefir_mem *mem, struct kefir_compiler_runner_configuration *options) {
    REQUIRE_OK(Actions[options->action](mem, options));
    return KEFIR_OK;
}

kefir_bool_t kefir_report_error(FILE *output, kefir_result_t res,
                                const struct kefir_compiler_runner_configuration *options) {
    if (res == KEFIR_OK) {
        return true;
    } else {
        switch (options->error_report_type) {
            case KEFIR_COMPILER_RUNNER_ERROR_REPORT_TABULAR:
                fprintf(output, "Failed to compile! Error stack:\n");
                kefir_format_error_tabular(output, kefir_current_error());
                break;

            case KEFIR_COMPILER_RUNNER_ERROR_REPORT_JSON:
                kefir_format_error_json(output, kefir_current_error());
                break;
        }
        return false;
    }
}
