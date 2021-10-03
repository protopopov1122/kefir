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

#include "kefir/parser/builtins.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

static const struct {
    const char *identifier;
    kefir_ast_builtin_operator_t builtin_op;
} BUILTINS[] = {{KEFIR_PARSER_BUILTIN_VA_START, KEFIR_AST_BUILTIN_VA_START},
                {KEFIR_PARSER_BUILTIN_VA_END, KEFIR_AST_BUILTIN_VA_END},
                {KEFIR_PARSER_BUILTIN_VA_COPY, KEFIR_AST_BUILTIN_VA_COPY},
                {KEFIR_PARSER_BUILTIN_VA_ARG, KEFIR_AST_BUILTIN_VA_ARG}};
static const kefir_size_t BUILTIN_COUNT = sizeof(BUILTINS) / sizeof(BUILTINS[0]);

kefir_result_t kefir_parser_get_builtin_operation(const char *identifier, kefir_ast_builtin_operator_t *builtin_op) {
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(builtin_op != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to builtin operator"));

    for (kefir_size_t i = 0; i < BUILTIN_COUNT; i++) {
        if (strcmp(BUILTINS[i].identifier, identifier) == 0) {
            *builtin_op = BUILTINS[i].builtin_op;
            return KEFIR_OK;
        }
    }
    return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Provided identifier is not a builtin");
}
