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

static kefir_result_t scan_return(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    struct kefir_parser *parser = builder->parser;
    REQUIRE_OK(PARSER_SHIFT(parser));
    if (!PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON)) {
        kefir_result_t res;
        REQUIRE_MATCH_OK(
            &res, kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, expression), NULL),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected expression"));
        REQUIRE_OK(kefir_parser_ast_builder_return_value_statement(mem, builder));
    } else {
        REQUIRE_OK(kefir_parser_ast_builder_return_statement(mem, builder));
    }
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected semicolon"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

static kefir_result_t scan_goto(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    struct kefir_parser *parser = builder->parser;
    REQUIRE_OK(PARSER_SHIFT(parser));

    if (PARSER_TOKEN_IS_IDENTIFIER(parser, 0) || !parser->configuration->label_addressing) {
        REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected identifier"));

        const char *identifier = kefir_parser_token_cursor_at(parser->cursor, 0)->identifier;
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_goto_statement(mem, builder, identifier));
        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected semicolon"));
        REQUIRE_OK(PARSER_SHIFT(parser));
    } else {
        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_STAR),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Expected either identifier or dereferenced address"));
        REQUIRE_OK(PARSER_SHIFT(parser));

        kefir_result_t res = KEFIR_OK;
        REQUIRE_MATCH_OK(
            &res, kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, cast_expression), NULL),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected cast expression"));

        REQUIRE_OK(kefir_parser_ast_builder_goto_address_statement(mem, builder));
        REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected semicolon"));
        REQUIRE_OK(PARSER_SHIFT(parser));
    }
    return KEFIR_OK;
}

static kefir_result_t scan_continue(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    struct kefir_parser *parser = builder->parser;
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(kefir_parser_ast_builder_continue_statement(mem, builder));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected semicolon"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

static kefir_result_t scan_break(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    struct kefir_parser *parser = builder->parser;
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(kefir_parser_ast_builder_break_statement(mem, builder));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected semicolon"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser AST builder"));
    struct kefir_parser *parser = builder->parser;

    if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_RETURN)) {
        REQUIRE_OK(scan_return(mem, builder));
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_GOTO)) {
        REQUIRE_OK(scan_goto(mem, builder));
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_BREAK)) {
        REQUIRE_OK(scan_break(mem, builder));
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_CONTINUE)) {
        REQUIRE_OK(scan_continue(mem, builder));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match jump statement");
    }

    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(jump_statement)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                           struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}
