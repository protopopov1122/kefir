/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KEFIR_PARSER_RULESET_H_
#define KEFIR_PARSER_RULESET_H_

#include "kefir/core/mem.h"

typedef struct kefir_ast_declarator_specifier_list kefir_ast_declarator_specifier_list_t;
typedef struct kefir_ast_declarator kefir_ast_declarator_t;
typedef struct kefir_ast_initializer_designation kefir_ast_initializer_designation_t;
typedef struct kefir_ast_initializer kefir_ast_initializer_t;
typedef struct kefir_ast_node_base kefir_ast_node_base_t;
typedef struct kefir_parser kefir_parser_t;

#define KEFIR_PARSER_RULESET_IDENTIFIER(_rule) KEFIR_PARSER_RULESET_##_rule

typedef enum kefir_parser_ruleset_identifier {
#define DECLARE_RULE(_rule) KEFIR_PARSER_RULESET_IDENTIFIER(_rule)
    DECLARE_RULE(identifier),
    DECLARE_RULE(constant),
    DECLARE_RULE(string_literal),
    DECLARE_RULE(generic_selection),
    DECLARE_RULE(compound_literal),
    DECLARE_RULE(primary_expression),
    DECLARE_RULE(postfix_expression),
    DECLARE_RULE(unary_expression),
    DECLARE_RULE(cast_expression),
    DECLARE_RULE(multiplicative_expression),
    DECLARE_RULE(additive_expression),
    DECLARE_RULE(shift_expression),
    DECLARE_RULE(relational_expression),
    DECLARE_RULE(equality_expression),
    DECLARE_RULE(bitwise_and_expression),
    DECLARE_RULE(bitwise_xor_expression),
    DECLARE_RULE(bitwise_or_expression),
    DECLARE_RULE(logical_and_expression),
    DECLARE_RULE(logical_or_expression),
    DECLARE_RULE(conditional_expression),
    DECLARE_RULE(expression),
    DECLARE_RULE(assignment_expression),
    DECLARE_RULE(constant_expression),
    DECLARE_RULE(type_name),
    DECLARE_RULE(static_assertion),
    DECLARE_RULE(declaration),
    DECLARE_RULE(labeled_statement),
    DECLARE_RULE(compound_statement),
    DECLARE_RULE(expression_statement),
    DECLARE_RULE(selection_statement),
    DECLARE_RULE(iteration_statement),
    DECLARE_RULE(jump_statement),
    DECLARE_RULE(statement),
    DECLARE_RULE(function_definition),
    DECLARE_RULE(external_declaration),
    DECLARE_RULE(translation_unit),
    KEFIR_PARSER_RULESET_PREDEFINED_COUNT
#undef DECLARE_RULE
} kefir_parser_ruleset_identifier_t;

#define KEFIR_PARSER_RULESET_EXTRA_SLOTS 128
#define KEFIR_PARSER_RULESET_CAPACITY (KEFIR_PARSER_RULESET_PREDEFINED_COUNT + KEFIR_PARSER_RULESET_EXTRA_SLOTS)
_Static_assert(KEFIR_PARSER_RULESET_CAPACITY >= KEFIR_PARSER_RULESET_PREDEFINED_COUNT,
               "Parser ruleset capacity cannot be less than the number of predefined rules");

typedef kefir_result_t (*kefir_parser_rule_fn_t)(struct kefir_mem *, struct kefir_parser *,
                                                 struct kefir_ast_node_base **, void *);

typedef struct kefir_parser_ruleset {
    kefir_parser_rule_fn_t rules[KEFIR_PARSER_RULESET_CAPACITY];

    kefir_result_t (*declaration_specifier)(struct kefir_mem *, struct kefir_parser *,
                                            struct kefir_ast_declarator_specifier_list *);
    kefir_result_t (*declaration_specifier_list)(struct kefir_mem *, struct kefir_parser *,
                                                 struct kefir_ast_declarator_specifier_list *);
    kefir_result_t (*declarator)(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_declarator **);
    kefir_result_t (*abstract_declarator)(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_declarator **);
    kefir_result_t (*initializer_designation)(struct kefir_mem *, struct kefir_parser *,
                                              struct kefir_ast_initializer_designation **);
    kefir_result_t (*initializer_list)(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_initializer **);
    kefir_result_t (*initializer)(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_initializer **);
} kefir_parser_ruleset_t;

kefir_result_t kefir_parser_ruleset_init(struct kefir_parser_ruleset *);

#endif
