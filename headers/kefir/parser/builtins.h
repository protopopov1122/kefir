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

#ifndef KEFIR_PARSER_BUILTINS_H_
#define KEFIR_PARSER_BUILTINS_H_

#include "kefir/parser/parser.h"
#include "kefir/ast/constants.h"

#define KEFIR_PARSER_BUILTIN_VA_START "__builtin_va_start"
#define KEFIR_PARSER_BUILTIN_VA_END "__builtin_va_end"
#define KEFIR_PARSER_BUILTIN_VA_ARG "__builtin_va_arg"
#define KEFIR_PARSER_BUILTIN_VA_COPY "__builtin_va_copy"

kefir_result_t kefir_parser_get_builtin(const char *, kefir_ast_builtin_operator_t *);

#endif
