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
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser AST builder"));
    struct kefir_parser *parser = builder->parser;

    REQUIRE(PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_GENERIC),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match generic selection"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected left parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, assignment_expression), NULL));
    REQUIRE_OK(kefir_parser_ast_builder_generic_selection(mem, builder));

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected comma"));
    while (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        kefir_bool_t default_clause = false;
        kefir_result_t res = kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, type_name), NULL);
        if (res == KEFIR_NO_MATCH) {
            REQUIRE(PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_DEFAULT),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                           "Expected either type name of default keyword"));
            REQUIRE_OK(PARSER_SHIFT(parser));
            default_clause = true;
        } else {
            REQUIRE_OK(res);
        }

        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COLON),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected colon"));
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(
            kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, assignment_expression), NULL));
        if (default_clause) {
            REQUIRE_OK(kefir_parser_ast_builder_generic_selection_append_default(mem, builder));
        } else {
            REQUIRE_OK(kefir_parser_ast_builder_generic_selection_append(mem, builder));
        }
    }

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(generic_selection)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                              struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}
