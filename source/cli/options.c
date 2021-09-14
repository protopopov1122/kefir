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

kefir_result_t kefir_cli_parse_options(struct kefir_cli_options *options, char *const *argv, kefir_size_t argc) {
    REQUIRE(options != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to cli options"));
    REQUIRE(argv != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid argument list"));

    optind = 0;
    opterr = 0;
    *options = (struct kefir_cli_options){.output_type = KEFIR_CLI_OUTPUT_ASSEMBLY};

    int long_option_index = 0;
    static const struct option long_options[] = {
        {"output", required_argument, NULL, 'o'}, {"dump-tokens", no_argument, NULL, 0},
        {"dump-ast", no_argument, NULL, 0},       {"dump-ir", no_argument, NULL, 0},
        {"dump-asm", no_argument, NULL, 0},       {"detailed-output", no_argument, NULL, 'D'},
        {"help", no_argument, NULL, 'h'},
    };
    const char *options_string = "+:o:Dh";

    for (int c = getopt_long(argc, argv, options_string, long_options, &long_option_index); c != -1;
         c = getopt_long(argc, argv, options_string, long_options, &long_option_index)) {

        switch (c) {
            case 0:
                switch (long_option_index) {
                    case 1:
                        options->output_type = KEFIR_CLI_OUTPUT_TOKENS;
                        break;

                    case 2:
                        options->output_type = KEFIR_CLI_OUTPUT_AST;
                        break;

                    case 3:
                        options->output_type = KEFIR_CLI_OUTPUT_IR;
                        break;

                    case 4:
                        options->output_type = KEFIR_CLI_OUTPUT_ASSEMBLY;
                        break;

                    default:
                        return KEFIR_SET_ERRORF(KEFIR_INTERNAL_ERROR, "Unexpected option %s", argv[optind - 1]);
                }
                break;

            case 'o':
                options->output_filepath = optarg;
                break;

            case 'D':
                options->detailed_output = true;
                break;

            case 'h':
                options->display_help = true;
                break;

            case '?':
                return KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Unknown option %s", argv[optind - 1]);

            case ':':
                return KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Expected parameter for %s option", argv[optind - 1]);
        }
    }

    for (kefir_size_t i = (kefir_size_t) optind; i < argc; i++) {
        REQUIRE(options->input_filepath == NULL,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Cannot specify more than one input file"));
        options->input_filepath = argv[optind];
    }

    return KEFIR_OK;
}
