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
    char *const argv[] = {"", "-o", "file.out", "--detailed-output", "--dump-ast", "test.c"};
    struct kefir_compiler_runner_configuration opts;
    ASSERT_OK(kefir_cli_parse_runner_configuration(&kft_mem, &opts, argv, sizeof(argv) / sizeof(argv[0])));

    ASSERT(opts.action == KEFIR_COMPILER_RUNNER_ACTION_DUMP_AST);
    ASSERT(opts.detailed_output);
    ASSERT(!opts.skip_preprocessor);
    ASSERT(opts.source_id == NULL);
    ASSERT(opts.output_filepath != NULL);
    ASSERT(strcmp(opts.output_filepath, "file.out") == 0);
    ASSERT(opts.input_filepath != NULL);
    ASSERT(strcmp(opts.input_filepath, "test.c") == 0);
    ASSERT_OK(kefir_compiler_runner_configuration_free(&kft_mem, &opts));
    ASSERT(opts.default_pp_timestamp);
}
END_CASE

DEFINE_CASE(cli_options2, "CLI - options #2") {
    char *const argv[] = {"",   "--detailed-output", "--dump-tokens", "--dump-ir",
                          "-P", "--output",          "somefile",      "--pp-timestamp=10"};
    struct kefir_compiler_runner_configuration opts;
    ASSERT_OK(kefir_cli_parse_runner_configuration(&kft_mem, &opts, argv, sizeof(argv) / sizeof(argv[0])));

    ASSERT(opts.action == KEFIR_COMPILER_RUNNER_ACTION_DUMP_IR);
    ASSERT(opts.detailed_output);
    ASSERT(opts.skip_preprocessor);
    ASSERT(opts.source_id == NULL);
    ASSERT(opts.output_filepath != NULL);
    ASSERT(strcmp(opts.output_filepath, "somefile") == 0);
    ASSERT(opts.input_filepath == NULL);
    ASSERT_OK(kefir_compiler_runner_configuration_free(&kft_mem, &opts));
    ASSERT(!opts.default_pp_timestamp);
    ASSERT(opts.pp_timestamp == 10);
}
END_CASE

DEFINE_CASE(cli_options3, "CLI - options #3") {
    char *const argv[] = {"",         "-o",
                          "out.asm",  "--source-id=source1",
                          "--define", "X",
                          "-D",       "Y=X",
                          "--define", "TEST=   test123=test3,,,   ===",
                          "input.c"};
    struct kefir_compiler_runner_configuration opts;
    ASSERT_OK(kefir_cli_parse_runner_configuration(&kft_mem, &opts, argv, sizeof(argv) / sizeof(argv[0])));

    ASSERT(opts.action == KEFIR_COMPILER_RUNNER_ACTION_DUMP_ASSEMBLY);
    ASSERT(!opts.detailed_output);
    ASSERT(!opts.skip_preprocessor);
    ASSERT(opts.source_id != NULL);
    ASSERT(strcmp(opts.source_id, "source1") == 0);
    ASSERT(opts.output_filepath != NULL);
    ASSERT(strcmp(opts.output_filepath, "out.asm") == 0);
    ASSERT(opts.input_filepath != NULL);
    ASSERT(strcmp(opts.input_filepath, "input.c") == 0);

    struct kefir_hashtree_node *node = NULL;
    ASSERT_OK(kefir_hashtree_at(&opts.defines, (kefir_hashtree_key_t) "X", &node));
    ASSERT((const char *) node->value == NULL);
    ASSERT_OK(kefir_hashtree_at(&opts.defines, (kefir_hashtree_key_t) "Y", &node));
    ASSERT((const char *) node->value != NULL);
    ASSERT(strcmp((const char *) node->value, "X") == 0);
    ASSERT_OK(kefir_hashtree_at(&opts.defines, (kefir_hashtree_key_t) "TEST", &node));
    ASSERT((const char *) node->value != NULL);
    ASSERT(strcmp((const char *) node->value, "   test123=test3,,,   ===") == 0);
    ASSERT(!kefir_hashtree_has(&opts.defines, (kefir_hashtree_key_t) "Z"));
    ASSERT_OK(kefir_compiler_runner_configuration_free(&kft_mem, &opts));
    ASSERT(opts.default_pp_timestamp);
}
END_CASE

DEFINE_CASE(cli_options4, "CLI - options #4") {
    char *const argv[] = {"", "--help"};
    struct kefir_compiler_runner_configuration opts;
    ASSERT_OK(kefir_cli_parse_runner_configuration(&kft_mem, &opts, argv, sizeof(argv) / sizeof(argv[0])));

    ASSERT(opts.action == KEFIR_COMPILER_RUNNER_ACTION_HELP);
    ASSERT(!opts.detailed_output);
    ASSERT(!opts.skip_preprocessor);
    ASSERT(opts.source_id == NULL);
    ASSERT(opts.output_filepath == NULL);
    ASSERT(opts.input_filepath == NULL);
    ASSERT_OK(kefir_compiler_runner_configuration_free(&kft_mem, &opts));
    ASSERT(opts.default_pp_timestamp);
}
END_CASE
