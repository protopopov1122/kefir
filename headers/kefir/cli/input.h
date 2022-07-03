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

#ifndef KEFIR_CLI_INPUT_H_
#define KEFIR_CLI_INPUT_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include <stdio.h>

typedef struct kefir_cli_input {
    const char *filepath;

    kefir_size_t length;
    void *content;
} kefir_cli_input_t;

kefir_result_t kefir_cli_input_open(struct kefir_mem *, struct kefir_cli_input *, const char *, FILE *);
kefir_result_t kefir_cli_input_close(struct kefir_mem *, struct kefir_cli_input *);
kefir_result_t kefir_cli_input_get(const struct kefir_cli_input *, const char **, kefir_size_t *);

#endif
