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

typedef enum kefir_cli_output_type {
    KEFIR_CLI_OUTPUT_TOKENS,
    KEFIR_CLI_OUTPUT_AST,
    KEFIR_CLI_OUTPUT_IR,
    KEFIR_CLI_OUTPUT_ASSEMBLY
} kefir_cli_output_type_t;

typedef struct kefir_cli_options {
    const char *input_filepath;
    const char *output_filepath;
    kefir_cli_output_type_t output_type;
    kefir_bool_t detailed_output;
    kefir_bool_t display_help;
} kefir_cli_options_t;

kefir_result_t kefir_cli_parse_options(struct kefir_cli_options *, char *const *, kefir_size_t);

#endif
