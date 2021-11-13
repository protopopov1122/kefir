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

#include "kefir/ast/downcast.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_downcast_init_declarator(const struct kefir_ast_node_base *node,
                                                  struct kefir_ast_init_declarator **init_declarator) {
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node"));
    REQUIRE(init_declarator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST init declarator"));

    REQUIRE(node->klass->type == KEFIR_AST_INIT_DECLARATOR,
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to downcast AST node to init declarator"));
    *init_declarator = node->self;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_downcast_declaration(const struct kefir_ast_node_base *node,
                                              struct kefir_ast_declaration **declaration) {
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node"));
    REQUIRE(declaration != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST declaration"));

    REQUIRE(node->klass->type == KEFIR_AST_DECLARATION,
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to downcast AST node to init declarator"));
    *declaration = node->self;
    return KEFIR_OK;
}
