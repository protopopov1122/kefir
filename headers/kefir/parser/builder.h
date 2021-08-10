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

#ifndef KEFIR_PARSER_BUILDER_H_
#define KEFIR_PARSER_BUILDER_H_

#include "kefir/ast/node.h"
#include "kefir/parser/parser.h"

typedef struct kefir_parser_ast_builder {
    struct kefir_parser *parser;
    struct kefir_list stack;
} kefir_parser_ast_builder_t;

kefir_result_t kefir_parser_ast_builder_init(struct kefir_parser_ast_builder *, struct kefir_parser *);
kefir_result_t kefir_parser_ast_builder_free(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_push(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                             struct kefir_ast_node_base *);
kefir_result_t kefir_parser_ast_builder_pop(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                            struct kefir_ast_node_base **);
kefir_result_t kefir_parser_ast_builder_scan(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                             kefir_parser_rule_fn_t, void *);

typedef kefir_result_t kefir_parser_ast_builder_callback_t(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                           void *);
kefir_result_t kefir_parser_ast_builder_wrap(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **,
                                             kefir_parser_ast_builder_callback_t, void *);

kefir_result_t kefir_parser_ast_builder_array_subscript(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_function_call(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_function_call_append(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_struct_member(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                      kefir_bool_t, const char *);
kefir_result_t kefir_parser_ast_builder_unary_operation(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                        kefir_ast_unary_operation_type_t);
kefir_result_t kefir_parser_ast_builder_binary_operation(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                         kefir_ast_binary_operation_type_t);
kefir_result_t kefir_parser_ast_builder_cast(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_conditional_operator(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_assignment_operator(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                            kefir_ast_assignment_operation_t);
kefir_result_t kefir_parser_ast_builder_comma_operator(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_static_assertion(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_generic_selection(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_generic_selection_append(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_generic_selection_append_default(struct kefir_mem *,
                                                                         struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_compound_literal(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                         struct kefir_ast_initializer *);
kefir_result_t kefir_parser_ast_builder_declaration_list(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                         struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_parser_ast_builder_declaration(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                    struct kefir_ast_declarator *, struct kefir_ast_initializer *);

#endif
