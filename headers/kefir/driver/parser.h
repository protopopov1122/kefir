/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

#ifndef KEFIR_DRIVER_PARSER_H_
#define KEFIR_DRIVER_PARSER_H_

#include "kefir/core/mem.h"
#include "kefir/driver/configuration.h"
#include <stdio.h>

typedef enum kefir_driver_command {
    KEFIR_DRIVER_COMMAND_RUN,
    KEFIR_DRIVER_COMMAND_HELP,
    KEFIR_DRIVER_COMMAND_VERSION
} kefir_driver_command_t;

kefir_result_t kefir_driver_parse_args(struct kefir_mem *, struct kefir_symbol_table *,
                                       struct kefir_driver_configuration *, const char *const *, kefir_size_t,
                                       kefir_driver_command_t *, FILE *);

#endif
