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

#ifndef KEFIR_AST_DOWNCAST_H_
#define KEFIR_AST_DOWNCAST_H_

#include "kefir/ast/node.h"

#define DECLARE_DOWNCAST(_id, _node_type) \
    kefir_result_t kefir_ast_downcast_##_id(const struct kefir_ast_node_base *, _node_type **, kefir_bool_t)

DECLARE_DOWNCAST(init_declarator, struct kefir_ast_init_declarator);
DECLARE_DOWNCAST(declaration, struct kefir_ast_declaration);
DECLARE_DOWNCAST(any_struct_member, struct kefir_ast_struct_member);
DECLARE_DOWNCAST(translation_unit, struct kefir_ast_translation_unit);
DECLARE_DOWNCAST(identifier, struct kefir_ast_identifier);
DECLARE_DOWNCAST(compound_statement, struct kefir_ast_compound_statement);
DECLARE_DOWNCAST(type_name, struct kefir_ast_type_name);
DECLARE_DOWNCAST(function_definition, struct kefir_ast_function_definition);
DECLARE_DOWNCAST(static_assertion, struct kefir_ast_static_assertion);
DECLARE_DOWNCAST(extension_node, struct kefir_ast_extension_node);

#undef DECLARE_DOWNCAST

#endif
