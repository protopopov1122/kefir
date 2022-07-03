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

#include "kefir/cli/options.h"
#include "kefir/cli/parser.h"
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

    kefir_size_t identifier_length = iter - arg;
    char *identifier = KEFIR_MALLOC(mem, identifier_length + 1);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate macro identifier"));
    strncpy(identifier, arg, identifier_length);
    identifier[identifier_length] = '\0';
    const char *value = arg[identifier_length] != '\0' ? arg + identifier_length + 1 : NULL;
    REQUIRE_OK(kefir_hashtree_insert(mem, &options->defines, (kefir_hashtree_key_t) identifier,
                                     (kefir_hashtree_value_t) value));
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

static struct kefir_cli_option Options[] = {
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
    CUSTOM('I', "include-dir", true, include_hook),
    CUSTOM(0, "include", true, include_file_hook),
    SIMPLE('h', "help", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, KEFIR_COMPILER_RUNNER_ACTION_HELP, action),
    SIMPLE('v', "version", false, KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT, KEFIR_COMPILER_RUNNER_ACTION_VERSION,
           action),

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

    CODEGEN("emulated-tls", codegen.emulated_tls)

#undef SIMPLE
#undef PREHOOK
#undef POSTHOOK
#undef CUSTOM
#undef FEATURE
#undef CODEGEN
};

static kefir_result_t free_define_identifier(struct kefir_mem *mem, struct kefir_hashtree *tree,
                                             kefir_hashtree_key_t key, kefir_hashtree_value_t value, void *payload) {
    UNUSED(tree);
    UNUSED(value);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(char *, identifier, key);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));

    KEFIR_FREE(mem, identifier);
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_runner_configuration_init(struct kefir_compiler_runner_configuration *options) {
    REQUIRE(options != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to cli options"));

    *options =
        (struct kefir_compiler_runner_configuration){.action = KEFIR_COMPILER_RUNNER_ACTION_DUMP_ASSEMBLY,
                                                     .error_report_type = KEFIR_COMPILER_RUNNER_ERROR_REPORT_TABULAR,
                                                     .skip_preprocessor = false,
                                                     .default_pp_timestamp = true,
                                                     .features = {false},
                                                     .codegen = {false}};
    REQUIRE_OK(kefir_list_init(&options->include_path));
    REQUIRE_OK(kefir_list_init(&options->include_files));
    REQUIRE_OK(kefir_hashtree_init(&options->defines, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&options->defines, free_define_identifier, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_cli_parse_runner_configuration(struct kefir_mem *mem,
                                                    struct kefir_compiler_runner_configuration *options,
                                                    char *const *argv, kefir_size_t argc) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(options != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to cli options"));
    REQUIRE(argv != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid argument list"));

    kefir_size_t positional_args = argc;
    REQUIRE_OK(kefir_parse_cli_options(mem, options, &positional_args, Options, sizeof(Options) / sizeof(Options[0]),
                                       argv, argc));
    if (positional_args < argc) {
        REQUIRE(positional_args + 1 == argc,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "Cannot specify more than one input file"));
        options->input_filepath = argv[positional_args];
    }
    return KEFIR_OK;
}

kefir_result_t kefir_compiler_runner_configuration_free(struct kefir_mem *mem,
                                                        struct kefir_compiler_runner_configuration *options) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(options != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to cli options"));

    REQUIRE_OK(kefir_list_free(mem, &options->include_files));
    REQUIRE_OK(kefir_list_free(mem, &options->include_path));
    REQUIRE_OK(kefir_hashtree_free(mem, &options->defines));
    return KEFIR_OK;
}
