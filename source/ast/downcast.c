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

#define DEFINE_DOWNCAST(_id, _node_type, _cond, _err)                                                         \
    kefir_result_t kefir_ast_downcast_##_id(const struct kefir_ast_node_base *node, _node_type **result,      \
                                            kefir_bool_t owning) {                                            \
        UNUSED(owning);                                                                                       \
        REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node"));           \
        REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to " _err)); \
                                                                                                              \
        REQUIRE((_cond), KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to downcast AST node to " _err));            \
        *result = node->self;                                                                                 \
        return KEFIR_OK;                                                                                      \
    }

DEFINE_DOWNCAST(init_declarator, struct kefir_ast_init_declarator, node->klass->type == KEFIR_AST_INIT_DECLARATOR,
                "AST init declarator")
DEFINE_DOWNCAST(declaration, struct kefir_ast_declaration, node->klass->type == KEFIR_AST_DECLARATION,
                "AST declaration")
DEFINE_DOWNCAST(any_struct_member, struct kefir_ast_struct_member,
                node->klass->type == KEFIR_AST_STRUCTURE_MEMBER ||
                    node->klass->type == KEFIR_AST_STRUCTURE_INDIRECT_MEMBER,
                "AST structure member")
DEFINE_DOWNCAST(translation_unit, struct kefir_ast_translation_unit, node->klass->type == KEFIR_AST_TRANSLATION_UNIT,
                "AST translation unit")
DEFINE_DOWNCAST(identifier, struct kefir_ast_identifier, node->klass->type == KEFIR_AST_IDENTIFIER, "AST identifier")
DEFINE_DOWNCAST(compound_statement, struct kefir_ast_compound_statement,
                node->klass->type == KEFIR_AST_COMPOUND_STATEMENT, "AST compound statement")
DEFINE_DOWNCAST(type_name, struct kefir_ast_type_name, node->klass->type == KEFIR_AST_TYPE_NAME, "AST type name")
DEFINE_DOWNCAST(function_definition, struct kefir_ast_function_definition,
                node->klass->type == KEFIR_AST_FUNCTION_DEFINITION, "AST function definition")
DEFINE_DOWNCAST(static_assertion, struct kefir_ast_static_assertion, node->klass->type == KEFIR_AST_STATIC_ASSERTION,
                "AST static assertion")
DEFINE_DOWNCAST(extension_node, struct kefir_ast_extension_node, node->klass->type == KEFIR_AST_EXTENSION_NODE,
                "AST extension node")
DEFINE_DOWNCAST(expression_statement, struct kefir_ast_expression_statement,
                node->klass->type == KEFIR_AST_EXPRESSION_STATEMENT, "AST expression statement node")

#undef DEFINE_DOWNCAST
