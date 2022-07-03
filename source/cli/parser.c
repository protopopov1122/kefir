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

#include "kefir/cli/parser.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/core/string_buffer.h"
#include <getopt.h>
#include <string.h>

static kefir_result_t parse_impl_internal(struct kefir_mem *mem, void *data_obj, const struct kefir_cli_option *options,
                                          kefir_size_t option_count, char *const *argv, kefir_size_t argc) {
    const struct kefir_cli_option *short_option_map[1 << CHAR_BIT] = {0};
    struct kefir_string_buffer options_buf;
    REQUIRE_OK(kefir_string_buffer_init(mem, &options_buf, KEFIR_STRING_BUFFER_MULTIBYTE));
    struct option *long_options = KEFIR_MALLOC(mem, sizeof(struct option) * (option_count + 1));
    REQUIRE_ELSE(long_options != NULL, {
        kefir_string_buffer_free(mem, &options_buf);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate buffer for CLI options");
    });

    kefir_result_t res = KEFIR_OK;
    for (kefir_size_t i = 0; res == KEFIR_OK && i < option_count; i++) {
        if (options[i].short_option != '\0') {
            if (options_buf.length == 0) {
                REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, '+'));
                REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, ':'));
            }

            REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, options[i].short_option));
            if (options[i].has_argument) {
                REQUIRE_CHAIN(&res, kefir_string_buffer_insert_raw(mem, &options_buf, ':'));
            }

            short_option_map[(kefir_size_t) options[i].short_option] = &options[i];
        }
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, long_options);
        kefir_string_buffer_free(mem, &options_buf);
        return res;
    });
    const char *short_options = kefir_string_buffer_value(&options_buf, NULL);

    for (kefir_size_t i = 0; res == KEFIR_OK && i < option_count; i++) {
        long_options[i] =
            (struct option){options[i].long_option, options[i].has_argument ? required_argument : no_argument, NULL,
                            options[i].short_option};
    }
    long_options[option_count] = (struct option){0};

    int long_option_index = 0;
    for (int c = getopt_long(argc, argv, short_options, long_options, &long_option_index); res == KEFIR_OK && c != -1;
         c = getopt_long(argc, argv, short_options, long_options, &long_option_index)) {

        const struct kefir_cli_option *cli_option = NULL;
        if (c == '?') {
            res = KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Unknown option %s", argv[optind - 1]);
        } else if (c == ':') {
            res = KEFIR_SET_ERRORF(KEFIR_UI_ERROR, "Expected parameter for %s option", argv[optind - 1]);
        } else if (c != 0) {
            cli_option = short_option_map[c];
            REQUIRE_CHAIN_SET(&res, cli_option != NULL,
                              KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "CLI option is ill-defined"));
        } else {
            cli_option = &options[long_option_index];
        }

        if (res == KEFIR_OK && cli_option->prehook != NULL) {
            REQUIRE_CHAIN(&res, cli_option->prehook(mem, cli_option, data_obj, optarg));
        }

        if (res == KEFIR_OK) {
            void *param = (void *) (((kefir_uptr_t) data_obj) + cli_option->param_offset);
            switch (cli_option->action) {
                case KEFIR_CLI_OPTION_ACTION_NONE:
                    // Intentionally left blank
                    break;

                case KEFIR_CLI_OPTION_ACTION_ASSIGN_STRARG:
                    REQUIRE_CHAIN_SET(&res, cli_option->param_size == sizeof(const char *),
                                      KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "CLI option is ill-defined"));
                    *(const char **) param = optarg;
                    break;

                case KEFIR_CLI_OPTION_ACTION_ASSIGN_UINTARG:
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

                case KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT:
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
            REQUIRE_CHAIN(&res, cli_option->posthook(mem, cli_option, data_obj, optarg));
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

kefir_result_t kefir_parse_cli_options(struct kefir_mem *mem, void *data_obj, kefir_size_t *positional_args,
                                       const struct kefir_cli_option *options, kefir_size_t option_count,
                                       char *const *argv, kefir_size_t argc) {
    optind = 0;
    opterr = 0;
    REQUIRE_OK(parse_impl_internal(mem, data_obj, options, option_count, argv, argc));
    ASSIGN_PTR(positional_args, optind);
    return KEFIR_OK;
}
