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
#include "kefir/test/unit_test.h"
#include <string.h>

DEFINE_CASE(cli_options1, "CLI - options #1") {
    char *const argv[] = {"", "-o", "file.out", "-D", "--dump-ast", "test.c"};
    struct kefir_cli_options opts;
    ASSERT_OK(kefir_cli_parse_options(&opts, argv, sizeof(argv) / sizeof(argv[0])));

    ASSERT(opts.output_type == KEFIR_CLI_OUTPUT_AST);
    ASSERT(opts.detailed_output);
    ASSERT(opts.output_filepath != NULL);
    ASSERT(strcmp(opts.output_filepath, "file.out") == 0);
    ASSERT(opts.input_filepath != NULL);
    ASSERT(strcmp(opts.input_filepath, "test.c") == 0);
}
END_CASE

DEFINE_CASE(cli_options2, "CLI - options #2") {
    char *const argv[] = {"", "--detailed-output", "--dump-tokens", "--dump-ir", "--output", "somefile"};
    struct kefir_cli_options opts;
    ASSERT_OK(kefir_cli_parse_options(&opts, argv, sizeof(argv) / sizeof(argv[0])));

    ASSERT(opts.output_type == KEFIR_CLI_OUTPUT_IR);
    ASSERT(opts.detailed_output);
    ASSERT(opts.output_filepath != NULL);
    ASSERT(strcmp(opts.output_filepath, "somefile") == 0);
    ASSERT(opts.input_filepath == NULL);
}
END_CASE

DEFINE_CASE(cli_options3, "CLI - options #3") {
    char *const argv[] = {"", "-o", "out.asm", "input.c"};
    struct kefir_cli_options opts;
    ASSERT_OK(kefir_cli_parse_options(&opts, argv, sizeof(argv) / sizeof(argv[0])));

    ASSERT(opts.output_type == KEFIR_CLI_OUTPUT_ASSEMBLY);
    ASSERT(!opts.detailed_output);
    ASSERT(opts.output_filepath != NULL);
    ASSERT(strcmp(opts.output_filepath, "out.asm") == 0);
    ASSERT(opts.input_filepath != NULL);
    ASSERT(strcmp(opts.input_filepath, "input.c") == 0);
}
END_CASE
