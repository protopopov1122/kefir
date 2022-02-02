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
#include <getopt.h>
#include <string.h>

static kefir_result_t parse_impl(struct kefir_mem *mem, struct kefir_cli_options *options, char *const *argv,
                                 kefir_size_t argc) {
    int long_option_index = 0;
    static const struct option long_options[] = {{"output", required_argument, NULL, 'o'},
                                                 {"preprocess", no_argument, NULL, 'p'},
                                                 {"skip-preprocessor", no_argument, NULL, 'P'},
                                                 {"dump-tokens", no_argument, NULL, 0},
                                                 {"dump-ast", no_argument, NULL, 0},
                                                 {"dump-ir", no_argument, NULL, 0},
                                                 {"dump-asm", no_argument, NULL, 0},
                                                 {"json-errors", no_argument, NULL, 0},
                                                 {"tabular-errors", no_argument, NULL, 0},
                                                 {"target-profile", required_argument, NULL, 0},
                                                 {"source-id", required_argument, NULL, 0},
                                                 {"detailed-output", no_argument, NULL, 0},
                                                 {"pp-timestamp", required_argument, NULL, 0},
                                                 {"analyzer-non-strict-qualifiers", no_argument, NULL, 0},
                                                 {"analyzer-signed-enums", no_argument, NULL, 0},
                                                 {"parser-fail-on-attributes", no_argument, NULL, 0},
                                                 {"parser-implicit-function-def-int", no_argument, NULL, 0},
                                                 {"define", required_argument, NULL, 'D'},
                                                 {"include-dir", required_argument, NULL, 'I'},
                                                 {"help", no_argument, NULL, 'h'},
                                                 {"version", no_argument, NULL, 'v'},
                                                 {0}};
    const char *options_string = "+:o:I:D:pPhv";

    for (int c = getopt_long(argc, argv, options_string, long_options, &long_option_index); c != -1;
         c = getopt_long(argc, argv, options_string, long_options, &long_option_index)) {

        switch (c) {
            case 0:
                switch (long_option_index) {
                    case 3:
                        options->action = KEFIR_CLI_ACTION_DUMP_TOKENS;
                        break;

                    case 4:
                        options->action = KEFIR_CLI_ACTION_DUMP_AST;
                        break;

                    case 5:
                        options->action = KEFIR_CLI_ACTION_DUMP_IR;
                        break;

                    case 6:
                        options->action = KEFIR_CLI_ACTION_DUMP_ASSEMBLY;
                        break;

                    case 7:
                        options->error_report_type = KEFIR_CLI_ERROR_REPORT_JSON;
                        break;

                    case 8:
                        options->error_report_type = KEFIR_CLI_ERROR_REPORT_TABULAR;
                        break;

                    case 9:
                        options->target_profile = optarg;
                        break;

                    case 10:
                        options->source_id = optarg;
                        break;

                    case 11:
                        options->detailed_output = true;
                        break;

                    case 12:
                        options->pp_timestamp = strtoull(optarg, NULL, 10);
                        options->default_pp_timestamp = false;
                        break;

                    case 13:
                        options->analysis.non_strict_qualifiers = true;
                        break;

                    case 14:
                        options->analysis.signed_enum_type = true;
                        break;

                    case 15:
                        options->parser.fail_on_attributes = true;
                        break;

                    case 16:
                        options->parser.implicit_function_def_int = true;
                        break;

                    default:
                        return KEFIR_SET_ERRORF(KEFIR_INTERNAL_ERROR, "Unexpected option %s", argv[optind - 1]);
                }
                break;

            case 'o':
                options->output_filepath = optarg;
                break;

            case 'D': {
                const char *iter = optarg;
                while (*iter != '\0' && *iter != '=') {
                    ++iter;
                }

                kefir_size_t identifier_length = iter - optarg;
                char *identifier = KEFIR_MALLOC(mem, identifier_length + 1);
                REQUIRE(identifier != NULL,
                        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate macro identifier"));
                strncpy(identifier, optarg, identifier_length);
                identifier[identifier_length] = '\0';
                const char *value = optarg[identifier_length] != '\0' ? optarg + identifier_length + 1 : NULL;
                REQUIRE_OK(kefir_hashtree_insert(mem, &options->defines, (kefir_hashtree_key_t) identifier,
                                                 (kefir_hashtree_value_t) value));
            } break;

            case 'p':
                REQUIRE(
                    !options->skip_preprocessor,
                    KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Cannot combine %s with skipping preprocessor", argv[optind - 1]));
                options->action = KEFIR_CLI_ACTION_PREPROCESS;
                break;

            case 'P':
                REQUIRE(
                    options->action != KEFIR_CLI_ACTION_PREPROCESS,
                    KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Cannot combine %s with preprocessing action", argv[optind - 1]));
                options->skip_preprocessor = true;
                break;

            case 'I':
                REQUIRE_OK(kefir_list_insert_after(mem, &options->include_path, kefir_list_tail(&options->include_path),
                                                   optarg));
                break;

            case 'h':
                options->action = KEFIR_CLI_ACTION_HELP;
                break;

            case 'v':
                options->action = KEFIR_CLI_ACTION_VERSION;
                break;

            case '?':
                return KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Unknown option %s", argv[optind]);

            case ':':
                return KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Expected parameter for %s option", argv[optind - 1]);
        }
    }

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
