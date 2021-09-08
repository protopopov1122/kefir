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

#include "kefir/parser/rule_helpers.h"
#include "kefir/parser/builder.h"
#include "kefir/core/source_error.h"

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser AST builder"));
    struct kefir_parser *parser = builder->parser;

    REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, relational_expression), NULL));
    kefir_bool_t scan_additive = true;
    do {
        kefir_result_t res;
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_EQUAL)) {
            REQUIRE_OK(PARSER_SHIFT(parser));
            REQUIRE_MATCH_OK(
                &res,
                kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, relational_expression), NULL),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Expected relational expression"));
            REQUIRE_OK(kefir_parser_ast_builder_binary_operation(mem, builder, KEFIR_AST_OPERATION_EQUAL));
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_NOT_EQUAL)) {
            REQUIRE_OK(PARSER_SHIFT(parser));
            REQUIRE_MATCH_OK(
                &res,
                kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, relational_expression), NULL),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Expected relational expression"));
            REQUIRE_OK(kefir_parser_ast_builder_binary_operation(mem, builder, KEFIR_AST_OPERATION_NOT_EQUAL));
        } else {
            scan_additive = false;
        }
    } while (scan_additive);

    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(equality_expression)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                                struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}
