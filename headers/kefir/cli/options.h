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

#ifndef KEFIR_CLI_OPTIONS_H_
#define KEFIR_CLI_OPTIONS_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include <time.h>

typedef enum kefir_cli_action {
    KEFIR_CLI_ACTION_PREPROCESS,
    KEFIR_CLI_ACTION_DUMP_TOKENS,
    KEFIR_CLI_ACTION_DUMP_AST,
    KEFIR_CLI_ACTION_DUMP_IR,
    KEFIR_CLI_ACTION_DUMP_ASSEMBLY,
    KEFIR_CLI_ACTION_HELP,
    KEFIR_CLI_ACTION_VERSION
} kefir_cli_action_t;

typedef enum kefir_cli_error_report_type {
    KEFIR_CLI_ERROR_REPORT_TABULAR,
    KEFIR_CLI_ERROR_REPORT_JSON
} kefir_cli_error_report_type_t;

typedef struct kefir_cli_options {
    kefir_cli_action_t action;
    const char *input_filepath;
    const char *output_filepath;
    const char *target_profile;
    const char *source_id;
    kefir_cli_error_report_type_t error_report_type;
    kefir_bool_t detailed_output;
    kefir_bool_t skip_preprocessor;
    struct kefir_list include_path;
    struct kefir_hashtree defines;
    time_t pp_timestamp;
    kefir_bool_t default_pp_timestamp;

    struct {
        kefir_bool_t fail_on_attributes;
    } parser;

    struct {
        kefir_bool_t non_strict_qualifiers;
        kefir_bool_t signed_enum_type;
    } analysis;
} kefir_cli_options_t;

kefir_result_t kefir_cli_parse_options(struct kefir_mem *, struct kefir_cli_options *, char *const *, kefir_size_t);
kefir_result_t kefir_cli_options_free(struct kefir_mem *, struct kefir_cli_options *);

#endif
