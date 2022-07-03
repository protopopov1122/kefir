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

#ifndef KEFIR_CLI_PARSER_H_
#define KEFIR_CLI_PARSER_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"

typedef enum kefir_cli_option_action {
    KEFIR_CLI_OPTION_ACTION_NONE,
    KEFIR_CLI_OPTION_ACTION_ASSIGN_STRARG,
    KEFIR_CLI_OPTION_ACTION_ASSIGN_UINTARG,
    KEFIR_CLI_OPTION_ACTION_ASSIGN_CONSTANT
} kefir_cli_option_action_t;

typedef struct kefir_cli_option {
    char short_option;
    const char *long_option;
    kefir_bool_t has_argument;
    kefir_cli_option_action_t action;
    kefir_uint64_t action_param;
    kefir_size_t param_offset;
    kefir_size_t param_size;
    kefir_result_t (*prehook)(struct kefir_mem *, const struct kefir_cli_option *, void *, const char *);
    kefir_result_t (*posthook)(struct kefir_mem *, const struct kefir_cli_option *, void *, const char *);
    void *payload;
} kefir_cli_option_t;

kefir_result_t kefir_parse_cli_options(struct kefir_mem *, void *, kefir_size_t *, const struct kefir_cli_option *,
                                       kefir_size_t, char *const *, kefir_size_t);

#endif
