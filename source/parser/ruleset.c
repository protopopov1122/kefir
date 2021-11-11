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

#include "kefir/ast/node.h"
#include "kefir/parser/ruleset.h"
#include "kefir/parser/rules.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

kefir_result_t kefir_parser_ruleset_init(struct kefir_parser_ruleset *ruleset) {
    REQUIRE(ruleset != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to parser ruleset"));

    memset(ruleset, 0, sizeof(struct kefir_parser_ruleset));
#define DEFINE_RULE(_rule)                                                                           \
    do {                                                                                             \
        ruleset->rules[KEFIR_PARSER_RULESET_IDENTIFIER(_rule)] = KEFIR_PARSER_RULE_FN_PREFIX(_rule); \
    } while (0)
    DEFINE_RULE(identifier);
    DEFINE_RULE(constant);
    DEFINE_RULE(string_literal);
    DEFINE_RULE(generic_selection);
    DEFINE_RULE(compound_literal);
    DEFINE_RULE(primary_expression);
    DEFINE_RULE(postfix_expression);
    DEFINE_RULE(unary_expression);
    DEFINE_RULE(cast_expression);
    DEFINE_RULE(multiplicative_expression);
    DEFINE_RULE(additive_expression);
    DEFINE_RULE(shift_expression);
    DEFINE_RULE(relational_expression);
    DEFINE_RULE(equality_expression);
    DEFINE_RULE(bitwise_and_expression);
    DEFINE_RULE(bitwise_xor_expression);
    DEFINE_RULE(bitwise_or_expression);
    DEFINE_RULE(logical_and_expression);
    DEFINE_RULE(logical_or_expression);
    DEFINE_RULE(conditional_expression);
    DEFINE_RULE(expression);
    DEFINE_RULE(assignment_expression);
    DEFINE_RULE(constant_expression);
    DEFINE_RULE(type_name);
    DEFINE_RULE(static_assertion);
    DEFINE_RULE(declaration);
    DEFINE_RULE(labeled_statement);
    DEFINE_RULE(compound_statement);
    DEFINE_RULE(expression_statement);
    DEFINE_RULE(selection_statement);
    DEFINE_RULE(iteration_statement);
    DEFINE_RULE(jump_statement);
    DEFINE_RULE(statement);
    DEFINE_RULE(function_definition);
    DEFINE_RULE(external_declaration);
    DEFINE_RULE(translation_unit);
#undef DEFINE_RULE

    ruleset->declaration_specifier = kefir_parser_scan_declaration_specifier;
    ruleset->declaration_specifier_list = kefir_parser_scan_declaration_specifier_list;
    ruleset->declarator = kefir_parser_scan_declarator;
    ruleset->abstract_declarator = kefir_parser_scan_abstract_declarator;
    ruleset->initializer_designation = kefir_parser_scan_initializer_designation;
    ruleset->initializer_list = kefir_parser_scan_initializer_list;
    ruleset->initializer = kefir_parser_scan_initializer;
    return KEFIR_OK;
}
