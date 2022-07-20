/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

static kefir_result_t skip_qualifiers(struct kefir_parser *parser) {
    kefir_bool_t skip = true;
    while (skip) {
        if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_VOLATILE) ||
            PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_INLINE) ||
            PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_GOTO)) {
            PARSER_SHIFT(parser);
        } else {
            skip = false;
        }
    }
    return KEFIR_OK;
}

static kefir_result_t skip_operand(struct kefir_mem *mem, struct kefir_parser *parser) {
    if (PARSER_TOKEN_IS_LEFT_BRACKET(parser, 0)) {
        PARSER_SHIFT(parser);
        REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Expected output symbolic name identifier"));
        PARSER_SHIFT(parser);
        REQUIRE(PARSER_TOKEN_IS_RIGHT_BRACKET(parser, 0),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right bracker"));
        PARSER_SHIFT(parser);
    }

    REQUIRE(PARSER_TOKEN_IS_STRING_LITERAL(parser, 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                   "Expected output constraint string literal"));
    PARSER_SHIFT(parser);

    kefir_result_t res = KEFIR_OK;
    struct kefir_ast_node_base *expr = NULL;
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected left parenthese"));
    PARSER_SHIFT(parser);
    REQUIRE_MATCH_OK(
        &res, KEFIR_PARSER_RULE_APPLY(mem, parser, assignment_expression, &expr),
        KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected assignment expression"));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, expr));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right parenthese"));
    PARSER_SHIFT(parser);

    return KEFIR_OK;
}

static kefir_result_t skip_goto(struct kefir_parser *parser) {
    kefir_bool_t skip = true;
    while (skip) {
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
            skip = false;
        } else {
            REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                           "Expected goto identifier"));
            PARSER_SHIFT(parser);
            if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
                PARSER_SHIFT(parser);
            }
        }
    }
    return KEFIR_OK;
}

static kefir_result_t skip_clobbers(struct kefir_parser *parser) {
    kefir_bool_t skip = true;
    while (skip) {
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COLON)) {
            PARSER_SHIFT(parser);
            REQUIRE_OK(skip_goto(parser));
            skip = false;
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
            skip = false;
        } else {
            REQUIRE(PARSER_TOKEN_IS_STRING_LITERAL(parser, 0),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                           "Expected clobber string literal"));
            PARSER_SHIFT(parser);
            if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
                PARSER_SHIFT(parser);
            }
        }
    }
    return KEFIR_OK;
}

static kefir_result_t skip_input_operands(struct kefir_mem *mem, struct kefir_parser *parser) {
    kefir_bool_t skip = true;
    while (skip) {
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COLON)) {
            PARSER_SHIFT(parser);
            REQUIRE_OK(skip_clobbers(parser));
            skip = false;
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
            skip = false;
        } else {
            REQUIRE_OK(skip_operand(mem, parser));
            if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
                PARSER_SHIFT(parser);
            }
        }
    }
    return KEFIR_OK;
}

static kefir_result_t skip_output_operands(struct kefir_mem *mem, struct kefir_parser *parser) {
    kefir_bool_t skip = true;
    while (skip) {
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COLON)) {
            PARSER_SHIFT(parser);
            REQUIRE_OK(skip_input_operands(mem, parser));
            skip = false;
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
            skip = false;
        } else {
            REQUIRE_OK(skip_operand(mem, parser));
            if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
                PARSER_SHIFT(parser);
            }
        }
    }
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(assembly)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                     struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE(PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ASM),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match assembly directive"));
    REQUIRE(parser->configuration->skip_assembly_directives,
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                   "Assembly directive is not supported"));

    PARSER_SHIFT(parser);
    REQUIRE_OK(skip_qualifiers(parser));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected left parenthese"));
    PARSER_SHIFT(parser);

    REQUIRE(
        PARSER_TOKEN_IS_STRING_LITERAL(parser, 0),
        KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected assembler template"));
    PARSER_SHIFT(parser);

    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COLON)) {
        PARSER_SHIFT(parser);
        REQUIRE_OK(skip_output_operands(mem, parser));
    }

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right parenthese"));
    PARSER_SHIFT(parser);

    *result = NULL;
    return KEFIR_OK;
}
