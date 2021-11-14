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

kefir_result_t kefir_ast_downcast_init_declarator(const struct kefir_ast_node_base *,
                                                  struct kefir_ast_init_declarator **);
kefir_result_t kefir_ast_downcast_declaration(const struct kefir_ast_node_base *, struct kefir_ast_declaration **);
kefir_result_t kefir_ast_downcast_any_struct_member(const struct kefir_ast_node_base *,
                                                    struct kefir_ast_struct_member **);
kefir_result_t kefir_ast_downcast_translation_unit(const struct kefir_ast_node_base *,
                                                   struct kefir_ast_translation_unit **);
kefir_result_t kefir_ast_downcast_identifier(const struct kefir_ast_node_base *, struct kefir_ast_identifier **);
kefir_result_t kefir_ast_downcast_compound_statement(const struct kefir_ast_node_base *,
                                                     struct kefir_ast_compound_statement **);
kefir_result_t kefir_ast_downcast_type_name(const struct kefir_ast_node_base *, struct kefir_ast_type_name **);
kefir_result_t kefir_ast_downcast_function_definition(const struct kefir_ast_node_base *,
                                                      struct kefir_ast_function_definition **);
kefir_result_t kefir_ast_downcast_static_assertion(const struct kefir_ast_node_base *,
                                                   struct kefir_ast_static_assertion **);

#endif
