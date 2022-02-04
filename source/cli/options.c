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
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/string_buffer.h"
#include <getopt.h>
#include <string.h>

enum CliOptionAction {
    CLI_ACTION_NONE,
    CLI_ACTION_ASSIGN_STRARG,
    CLI_ACTION_ASSIGN_UINTARG,
    CLI_ACTION_ASSIGN_CONSTANT
};

struct CliOption {
    char short_option;
    const char *long_option;
    kefir_bool_t has_argument;
    enum CliOptionAction action;
    kefir_uint64_t action_param;
    kefir_size_t param_offset;
    kefir_size_t param_size;
    kefir_result_t (*prehook)(struct kefir_mem *, struct CliOption *, void *, const char *);
    kefir_result_t (*posthook)(struct kefir_mem *, struct CliOption *, void *, const char *);
};

#define MEMBERSZ(structure, field) (sizeof(((structure *) NULL)->field))

static kefir_result_t preprocess_hook(struct kefir_mem *mem, struct CliOption *option, void *raw_options,
                                      const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    UNUSED(arg);
    ASSIGN_DECL_CAST(struct kefir_cli_options *, options, raw_options);

    REQUIRE(!options->skip_preprocessor,
            KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Cannot combine %s with skipping preprocessor", option->long_option));
    return KEFIR_OK;
}

static kefir_result_t skip_preprocessor_hook(struct kefir_mem *mem, struct CliOption *option, void *raw_options,
                                             const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    UNUSED(arg);
    ASSIGN_DECL_CAST(struct kefir_cli_options *, options, raw_options);

    REQUIRE(options->action != KEFIR_CLI_ACTION_PREPROCESS,
            KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Cannot combine %s with preprocessing action", option->long_option));
    return KEFIR_OK;
}

static kefir_result_t pp_timestamp_hook(struct kefir_mem *mem, struct CliOption *option, void *raw_options,
                                        const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    UNUSED(arg);
    ASSIGN_DECL_CAST(struct kefir_cli_options *, options, raw_options);

    options->default_pp_timestamp = false;
    return KEFIR_OK;
}

static kefir_result_t define_hook(struct kefir_mem *mem, struct CliOption *option, void *raw_options, const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    ASSIGN_DECL_CAST(struct kefir_cli_options *, options, raw_options);

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

static kefir_result_t include_hook(struct kefir_mem *mem, struct CliOption *option, void *raw_options,
                                   const char *arg) {
    UNUSED(mem);
    UNUSED(option);
    ASSIGN_DECL_CAST(struct kefir_cli_options *, options, raw_options);

    REQUIRE_OK(
        kefir_list_insert_after(mem, &options->include_path, kefir_list_tail(&options->include_path), (void *) arg));
    return KEFIR_OK;
}

static struct CliOption Options[] = {
#define SIMPLE(short, long, has_arg, action, action_param, field)                              \
    {                                                                                          \
        short, long, has_arg, action, action_param, offsetof(struct kefir_cli_options, field), \
            MEMBERSZ(struct kefir_cli_options, field), NULL, NULL                              \
    }
#define PREHOOK(short, long, has_arg, action, action_param, field, hook)                       \
    {                                                                                          \
        short, long, has_arg, action, action_param, offsetof(struct kefir_cli_options, field), \
            MEMBERSZ(struct kefir_cli_options, field), hook, NULL                              \
    }
#define POSTHOOK(short, long, has_arg, action, action_param, field, hook)                      \
    {                                                                                          \
        short, long, has_arg, action, action_param, offsetof(struct kefir_cli_options, field), \
            MEMBERSZ(struct kefir_cli_options, field), NULL, hook                              \
    }
#define CUSTOM(short, long, has_arg, hook) \
    { short, long, has_arg, CLI_ACTION_NONE, 0, 0, 0, hook, NULL }

    SIMPLE('o', "output", true, CLI_ACTION_ASSIGN_STRARG, 0, output_filepath),
    PREHOOK('p', "preprocess", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ACTION_PREPROCESS, action, preprocess_hook),
    PREHOOK('P', "skip-preprocessor", false, CLI_ACTION_ASSIGN_CONSTANT, true, skip_preprocessor,
            skip_preprocessor_hook),
    SIMPLE(0, "dump-tokens", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ACTION_DUMP_TOKENS, action),
    SIMPLE(0, "dump-ast", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ACTION_DUMP_AST, action),
    SIMPLE(0, "dump-ir", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ACTION_DUMP_IR, action),
    SIMPLE(0, "json-errors", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ERROR_REPORT_JSON, error_report_type),
    SIMPLE(0, "tabular-errors", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ERROR_REPORT_TABULAR, error_report_type),
    SIMPLE(0, "target-profile", true, CLI_ACTION_ASSIGN_STRARG, 0, target_profile),
    SIMPLE(0, "source-id", true, CLI_ACTION_ASSIGN_STRARG, 0, source_id),
    SIMPLE(0, "detailed-output", false, CLI_ACTION_ASSIGN_CONSTANT, true, detailed_output),
    POSTHOOK(0, "pp-timestamp", true, CLI_ACTION_ASSIGN_UINTARG, 0, pp_timestamp, pp_timestamp_hook),

    CUSTOM('D', "define", true, define_hook),
    CUSTOM('I', "include-dir", true, include_hook),
    SIMPLE('h', "help", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ACTION_HELP, action),
    SIMPLE('v', "version", false, CLI_ACTION_ASSIGN_CONSTANT, KEFIR_CLI_ACTION_VERSION, action),

    SIMPLE(0, "analyzer-non-strict-qualifiers", false, CLI_ACTION_ASSIGN_CONSTANT, true,
           analysis.non_strict_qualifiers),
    SIMPLE(0, "analyzer-signed-enums", false, CLI_ACTION_ASSIGN_CONSTANT, true, analysis.signed_enum_type),
    SIMPLE(0, "analyzer-implicit-function-decl", false, CLI_ACTION_ASSIGN_CONSTANT, true,
           analysis.implicit_function_declaration),
    SIMPLE(0, "parser-fail-on-attributes", false, CLI_ACTION_ASSIGN_CONSTANT, true, parser.fail_on_attributes),
    SIMPLE(0, "parser-implicit-function-def-int", false, CLI_ACTION_ASSIGN_CONSTANT, true,
           parser.implicit_function_def_int)

#undef SIMPLE
#undef PREHOOK
#undef POSTHOOK
#undef CUSTOM
};

static kefir_result_t parse_impl_internal(struct kefir_mem *mem, struct kefir_cli_options *options, char *const *argv,
                                          kefir_size_t argc) {
    const kefir_size_t Option_Count = sizeof(Options) / sizeof(Options[0]);

    struct CliOption *short_option_map[1 << CHAR_BIT] = {0};
    struct kefir_string_buffer options_buf;
    REQUIRE_OK(kefir_string_buffer_init(mem, &options_buf, KEFIR_STRING_BUFFER_MULTIBYTE));
    struct option *long_options = KEFIR_MALLOC(mem, sizeof(struct option) * (Option_Count + 1));
    REQUIRE_ELSE(long_options != NULL, {
        kefir_string_buffer_free(mem, &options_buf);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate buffer for CLI options");
    });

    kefir_result_t res = KEFIR_OK;
    for (kefir_size_t i = 0; res == KEFIR_OK && i < Option_Count; i++) {
        if (Options[i].short_option != '\0') {
            if (options_buf.length == 0) {
                REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, '+'));
                REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, ':'));
            }

            REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, Options[i].short_option));
            if (Options[i].has_argument) {
                REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, ':'));
            }

            short_option_map[(kefir_size_t) Options[i].short_option] = &Options[i];
        }
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, long_options);
        kefir_string_buffer_free(mem, &options_buf);
        return res;
    });
    const char *short_options = kefir_string_buffer_value(&options_buf, NULL);

    for (kefir_size_t i = 0; res == KEFIR_OK && i < Option_Count; i++) {
        long_options[i] =
            (struct option){Options[i].long_option, Options[i].has_argument ? required_argument : no_argument, NULL,
                            Options[i].short_option};
    }
    long_options[Option_Count] = (struct option){0};

    int long_option_index = 0;
    for (int c = getopt_long(argc, argv, short_options, long_options, &long_option_index); res == KEFIR_OK && c != -1;
         c = getopt_long(argc, argv, short_options, long_options, &long_option_index)) {

        struct CliOption *cli_option = NULL;
        if (c == '?') {
            res = KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Unknown option %s", argv[optind - 1]);
        } else if (c == ':') {
            res = KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Expected parameter for %s option", argv[optind - 1]);
        } else if (c != 0) {
            cli_option = short_option_map[c];
            REQUIRE_CHAIN_SET(&res, cli_option != NULL,
                              KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "CLI option is ill-defined"));
        } else {
            cli_option = &Options[long_option_index];
        }

        if (res == KEFIR_OK && cli_option->prehook != NULL) {
            REQUIRE_CHAIN(&res, cli_option->prehook(mem, cli_option, options, optarg));
        }

        if (res == KEFIR_OK) {
            void *param = (void *) (((kefir_uptr_t) options) + cli_option->param_offset);
            switch (cli_option->action) {
                case CLI_ACTION_NONE:
                    // Intentionally left blank
                    break;

                case CLI_ACTION_ASSIGN_STRARG:
                    REQUIRE_CHAIN_SET(&res, cli_option->param_size == sizeof(const char *),
                                      KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "CLI option is ill-defined"));
                    *(const char **) param = optarg;
                    break;

                case CLI_ACTION_ASSIGN_UINTARG:
                    switch (cli_option->param_size) {
                        case sizeof(kefir_uint8_t):
                            *(kefir_uint8_t *) param = strtoull(optarg, NULL, 10);
                            break;

                        case sizeof(kefir_uint16_t):
                            *(kefir_uint16_t *) param = strtoull(optarg, NULL, 10);
                            break;

                        case sizeof(kefir_uint32_t):
                            *(kefir_uint32_t *) param = strtoull(optarg, NULL, 10);
                            break;

                        case sizeof(kefir_uint64_t):
                            *(kefir_uint64_t *) param = strtoull(optarg, NULL, 10);
                            break;

                        default:
                            res = KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "CLI option is ill-defined");
                            break;
                    }
                    break;

                case CLI_ACTION_ASSIGN_CONSTANT:
                    switch (cli_option->param_size) {
                        case sizeof(kefir_uint8_t):
                            *(kefir_uint8_t *) param = cli_option->action_param;
                            break;

                        case sizeof(kefir_uint16_t):
                            *(kefir_uint16_t *) param = cli_option->action_param;
                            break;

                        case sizeof(kefir_uint32_t):
                            *(kefir_uint32_t *) param = cli_option->action_param;
                            break;

                        case sizeof(kefir_uint64_t):
                            *(kefir_uint64_t *) param = cli_option->action_param;
                            break;

                        default:
                            res = KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "CLI option is ill-defined");
                            break;
                    }
                    break;
            }
        }

        if (res == KEFIR_OK && cli_option->posthook != NULL) {
            REQUIRE_CHAIN(&res, cli_option->posthook(mem, cli_option, options, optarg));
        }
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, long_options);
        kefir_string_buffer_free(mem, &options_buf);
        return res;
    });

    KEFIR_FREE(mem, long_options);
    REQUIRE_OK(kefir_string_buffer_free(mem, &options_buf));
    return KEFIR_OK;
}

static kefir_result_t parse_impl(struct kefir_mem *mem, struct kefir_cli_options *options, char *const *argv,
                                 kefir_size_t argc) {
    REQUIRE_OK(parse_impl_internal(mem, options, argv, argc));

    for (kefir_size_t i = (kefir_size_t) optind; i < argc; i++) {
        REQUIRE(options->input_filepath == NULL,
                KEFIR_SET_ERROR(KEFIR_UI_ERROR, "Cannot specify more than one input file"));
        options->input_filepath = argv[optind];
    }
    return KEFIR_OK;
}

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

kefir_result_t kefir_cli_parse_options(struct kefir_mem *mem, struct kefir_cli_options *options, char *const *argv,
                                       kefir_size_t argc) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(options != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to cli options"));
    REQUIRE(argv != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid argument list"));

    optind = 0;
    opterr = 0;
    *options = (struct kefir_cli_options){.action = KEFIR_CLI_ACTION_DUMP_ASSEMBLY,
                                          .error_report_type = KEFIR_CLI_ERROR_REPORT_TABULAR,
                                          .skip_preprocessor = false,
                                          .default_pp_timestamp = true,
                                          .parser = {.fail_on_attributes = false},
                                          .analysis = {.non_strict_qualifiers = false}};
    REQUIRE_OK(kefir_list_init(&options->include_path));
    REQUIRE_OK(kefir_hashtree_init(&options->defines, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&options->defines, free_define_identifier, NULL));
    kefir_result_t res = parse_impl(mem, options, argv, argc);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &options->include_path);
        kefir_hashtree_free(mem, &options->defines);
        return res;
    });

    return KEFIR_OK;
}

kefir_result_t kefir_cli_options_free(struct kefir_mem *mem, struct kefir_cli_options *options) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(options != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to cli options"));

    REQUIRE_OK(kefir_list_free(mem, &options->include_path));
    REQUIRE_OK(kefir_hashtree_free(mem, &options->defines));
    return KEFIR_OK;
}
