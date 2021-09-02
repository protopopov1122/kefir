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

#ifndef KEFIR_AST_FORMAT_H_
#define KEFIR_AST_FORMAT_H_

#include "kefir/ast/node.h"
#include "kefir/util/json.h"

kefir_result_t kefir_ast_format(struct kefir_json_output *, const struct kefir_ast_node_base *);
kefir_result_t kefir_ast_format_declarator_specifier_list(struct kefir_json_output *,
                                                          const struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_ast_format_declarator_specifier(struct kefir_json_output *,
                                                     const struct kefir_ast_declarator_specifier *);
kefir_result_t kefir_ast_format_declarator_specifier_list(struct kefir_json_output *,
                                                          const struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_ast_format_declarator(struct kefir_json_output *, const struct kefir_ast_declarator *);
kefir_result_t kefir_ast_format_initializer_designation(struct kefir_json_output *,
                                                        const struct kefir_ast_initializer_designation *);
kefir_result_t kefir_ast_format_initializer(struct kefir_json_output *, const struct kefir_ast_initializer *);

#endif
