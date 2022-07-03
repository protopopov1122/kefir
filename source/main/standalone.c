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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "kefir/compiler/compiler.h"
#include "kefir/main/runner.h"

// Standalone compiler without driver

int main(int argc, char *const *argv) {
    struct kefir_mem *mem = kefir_system_memalloc();
    struct kefir_compiler_runner_configuration options;

    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "C");
    kefir_result_t res = kefir_compiler_runner_configuration_init(&options);
    REQUIRE_CHAIN(&res, kefir_cli_parse_runner_configuration(mem, &options, argv, argc));
    REQUIRE_CHAIN(&res, kefir_run_compiler(mem, &options));
    REQUIRE_CHAIN(&res, kefir_compiler_runner_configuration_free(mem, &options));
    return kefir_report_error(res, &options) ? EXIT_SUCCESS : EXIT_FAILURE;
}
