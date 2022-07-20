/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

#include "kefir/driver/compiler_options.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

#define MEMBERSZ(structure, field) (sizeof(((structure *) NULL)->field))

static kefir_result_t preprocess_hook(struct kefir_mem *mem, const struct kefir_cli_option *option, void *raw_options,
                                      const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    UNUSED(arg);
    ASSIGN_DECL_CAST(struct kefir_compiler_runner_configuration *, options, raw_options);

    REQUIRE(!options->skip_preprocessor,
            KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Cannot combine %s with skipping preprocessor", option->long_option));
    return KEFIR_OK;
}

static kefir_result_t skip_preprocessor_hook(struct kefir_mem *mem, const struct kefir_cli_option *option,
                                             void *raw_options, const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    UNUSED(arg);
    ASSIGN_DECL_CAST(struct kefir_compiler_runner_configuration *, options, raw_options);

    REQUIRE(options->action != KEFIR_COMPILER_RUNNER_ACTION_PREPROCESS,
            KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Cannot combine %s with preprocessing action", option->long_option));
    return KEFIR_OK;
}

static kefir_result_t pp_timestamp_hook(struct kefir_mem *mem, const struct kefir_cli_option *option, void *raw_options,
                                        const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    UNUSED(arg);
    ASSIGN_DECL_CAST(struct kefir_compiler_runner_configuration *, options, raw_options);

    options->default_pp_timestamp = false;
    return KEFIR_OK;
}

static kefir_result_t define_hook(struct kefir_mem *mem, const struct kefir_cli_option *option, void *raw_options,
                                  const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    ASSIGN_DECL_CAST(struct kefir_compiler_runner_configuration *, options, raw_options);

    const char *iter = arg;
    while (*iter != '\0' && *iter != '=') {
        ++iter;
    }

    char macro_identifier[4097];
    kefir_size_t identifier_length = MIN((kefir_size_t) (iter - arg), sizeof(macro_identifier) - 1);
    strncpy(macro_identifier, arg, identifier_length);
    macro_identifier[identifier_length] = '\0';
    const char *value = *iter != '\0' ? iter + 1 : NULL;
    REQUIRE_OK(kefir_compiler_runner_configuration_define(mem, options, macro_identifier, value));
    return KEFIR_OK;
}

static kefir_result_t undefine_hook(struct kefir_mem *mem, const struct kefir_cli_option *option, void *raw_options,
                                    const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    ASSIGN_DECL_CAST(struct kefir_compiler_runner_configuration *, options, raw_options);

    REQUIRE_OK(kefir_list_insert_after(mem, &options->undefines, kefir_list_tail(&options->undefines), (void *) arg));
    if (kefir_hashtree_has(&options->defines, (kefir_hashtree_key_t) arg)) {
        REQUIRE_OK(kefir_hashtree_delete(mem, &options->defines, (kefir_hashtree_key_t) arg));
    }
    return KEFIR_OK;
}

static kefir_result_t include_hook(struct kefir_mem *mem, const struct kefir_cli_option *option, void *raw_options,
                                   const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    ASSIGN_DECL_CAST(struct kefir_compiler_runner_configuration *, options, raw_options);

    REQUIRE_OK(
        kefir_list_insert_after(mem, &options->include_path, kefir_list_tail(&options->include_path), (void *) arg));
    return KEFIR_OK;
}

static kefir_result_t include_file_hook(struct kefir_mem *mem, const struct kefir_cli_option *option, void *raw_options,
                                        const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    ASSIGN_DECL_CAST(struct kefir_compiler_runner_configuration *, options, raw_options);

    REQUIRE_OK(
        kefir_list_insert_after(mem, &options->include_files, kefir_list_tail(&options->include_files), (void *) arg));
    return KEFIR_OK;
}

struct kefir_cli_option KefirCompilerConfigurationOptions[] = {
#define SIMPLE(short, long, has_arg, action, action_param, field)                                                \
    {                                                                                                            \
        short, long, has_arg, action, action_param, offsetof(struct kefir_compiler_runner_configuration, field), \
            MEMBERSZ(struct kefir_compiler_runner_configuration, field), NULL, NULL, NULL                        \
    }
#define PREHOOK(short, long, has_arg, action, action_param, field, hook)                                         \
    {                                                                                                            \
        short, long, has_arg, action, action_param, offsetof(struct kefir_compiler_runner_configuration, field), \
            MEMBERSZ(struct kefir_compiler_runner_configuration, field), hook, NULL, NULL                        \
    }
#define POSTHOOK(short, long, has_arg, action, action_param, field, hook)                                        \
    {                                                                                                            \
        short, long, has_arg, action, action_param, offsetof(struct kefir_compiler_runner_configuration, field), \
            MEMBERSZ(struct kefir_compiler_runner_configuration, field), NULL, hook, NULL                        \
    }
#define CUSTOM(short, long, has_arg, hook) \
    { short, long, has_arg, KEFIR_CLI_OPTION_ACTION_NONE, 0, 0, 0, hook, NULL, NULL }
#define FEATURE(name, field)                                                                 \
    SIMPLE(0, "feature-" name, false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, true, field), \
        SIMPLE(0, "no-feature-" name, false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, false, field)
#define CODEGEN(name, field)                                                                 \
    SIMPLE(0, "codegen-" name, false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, true, field), \
        SIMPLE(0, "no-codegen-" name, false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, false, field)

    SIMPLE('o', "output", true, KEFIR_CLI_OPTION_ACTION_ASSIGN_STRARG, 0, output_filepath),
    PREHOOK('p', "preprocess", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, KEFIR_COMPILER_RUNNER_ACTION_PREPROCESS,
            action, preprocess_hook),
    PREHOOK('P', "skip-preprocessor", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, true, skip_preprocessor,
            skip_preprocessor_hook),
    SIMPLE(0, "dump-tokens", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, KEFIR_COMPILER_RUNNER_ACTION_DUMP_TOKENS,
           action),
    SIMPLE(0, "dump-ast", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, KEFIR_COMPILER_RUNNER_ACTION_DUMP_AST,
           action),
    SIMPLE(0, "dump-ir", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, KEFIR_COMPILER_RUNNER_ACTION_DUMP_IR, action),
    SIMPLE(0, "dump-runtime-code", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT,
           KEFIR_COMPILER_RUNNER_ACTION_DUMP_RUNTIME_CODE, action),
    SIMPLE(0, "json-errors", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, KEFIR_COMPILER_RUNNER_ERROR_REPORT_JSON,
           error_report_type),
    SIMPLE(0, "tabular-errors", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT,
           KEFIR_COMPILER_RUNNER_ERROR_REPORT_TABULAR, error_report_type),
    SIMPLE(0, "target-profile", true, KEFIR_CLI_OPTION_ACTION_ASSIGN_STRARG, 0, target_profile),
    SIMPLE(0, "source-id", true, KEFIR_CLI_OPTION_ACTION_ASSIGN_STRARG, 0, source_id),
    SIMPLE(0, "detailed-output", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, true, detailed_output),
    POSTHOOK(0, "pp-timestamp", true, KEFIR_CLI_OPTION_ACTION_ASSIGN_UINTARG, 0, pp_timestamp, pp_timestamp_hook),

    CUSTOM('D', "define", true, define_hook),
    CUSTOM('U', "undefine", true, undefine_hook),
    CUSTOM('I', "include-dir", true, include_hook),
    CUSTOM(0, "include", true, include_file_hook),

    FEATURE("non-strict-qualifiers", features.non_strict_qualifiers),
    FEATURE("signed-enums", features.signed_enum_type),
    FEATURE("implicit-function-decl", features.implicit_function_declaration),
    FEATURE("fail-on-attributes", features.fail_on_attributes),
    FEATURE("missing-function-return-type", features.missing_function_return_type),
    FEATURE("designated-init-colons", features.designated_initializer_colons),
    FEATURE("labels-as-values", features.labels_as_values),
    FEATURE("empty-structs", features.empty_structs),
    FEATURE("ext-pointer-arithmetics", features.ext_pointer_arithmetics),
    FEATURE("missing-braces-subobj", features.missing_braces_subobject),
    FEATURE("statement-expressions", features.statement_expressions),
    FEATURE("omitted-conditional-operand", features.omitted_conditional_operand),
    FEATURE("int-to-pointer", features.int_to_pointer),
    FEATURE("permissive-pointer-conv", features.permissive_pointer_conv),
    FEATURE("named-macro-vararg", features.named_macro_vararg),
    FEATURE("include-next", features.include_next),
    FEATURE("skip-assembly", features.skip_assembly),

    CODEGEN("emulated-tls", codegen.emulated_tls)

#undef SIMPLE
#undef PREHOOK
#undef POSTHOOK
#undef CUSTOM
#undef FEATURE
#undef CODEGEN
};

const kefir_size_t KefirCompilerConfigurationOptionCount =
    sizeof(KefirCompilerConfigurationOptions) / sizeof(KefirCompilerConfigurationOptions[0]);
