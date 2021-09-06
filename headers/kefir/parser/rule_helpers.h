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

#ifndef KEFIR_PARSER_RULE_HELPERS_H_
#define KEFIR_PARSER_RULE_HELPERS_H_

#include "kefir/parser/parser.h"
#include "kefir/parser/rules.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define APPLY_PROLOGUE(_mem, _parser, _result, _payload)                                                            \
    do {                                                                                                            \
        UNUSED((_payload));                                                                                         \
        REQUIRE((_mem) != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));       \
        REQUIRE((_parser) != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));              \
        REQUIRE((_result) != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST node")); \
    } while (0)

#define PARSER_CURSOR(_parser, _idx) (kefir_parser_token_cursor_at((_parser)->cursor, (_idx)))
#define PARSER_TOKEN_IS(_parser, _idx, _klass) (PARSER_CURSOR((_parser), (_idx))->klass == (_klass))
#define PARSER_TOKEN_IS_SENTINEL(_parser, _idx) PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_SENTINEL)
#define PARSER_TOKEN_IS_KEYWORD(_parser, _idx, _keyword) \
    (PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_KEYWORD) && PARSER_CURSOR((_parser), (_idx))->keyword == (_keyword))
#define PARSER_TOKEN_IS_IDENTIFIER(_parser, _idx) PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_IDENTIFIER)
#define PARSER_TOKEN_IS_CONSTANT(_parser, _idx, _type)           \
    (PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_CONSTANT) && \
     PARSER_CURSOR((_parser), (_idx))->constant.type == (_type))
#define PARSER_TOKEN_IS_STRING_LITERAL(_parser, _idx) PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_STRING_LITERAL)
#define PARSER_TOKEN_IS_PUNCTUATOR(_parser, _idx, _punctuator)     \
    (PARSER_TOKEN_IS((_parser), (_idx), KEFIR_TOKEN_PUNCTUATOR) && \
     PARSER_CURSOR((_parser), (_idx))->punctuator == (_punctuator))

#define PARSER_TOKEN_IS_LEFT_BRACKET(_parser, _idx)                                  \
    (PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_LEFT_BRACKET) || \
     PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACKET))
#define PARSER_TOKEN_IS_RIGHT_BRACKET(_parser, _idx)                                  \
    (PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_RIGHT_BRACKET) || \
     PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACKET))
#define PARSER_TOKEN_IS_LEFT_BRACE(_parser, _idx)                                  \
    (PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_LEFT_BRACE) || \
     PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACE))
#define PARSER_TOKEN_IS_RIGHT_BRACE(_parser, _idx)                                  \
    (PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_RIGHT_BRACE) || \
     PARSER_TOKEN_IS_PUNCTUATOR((_parser), (_idx), KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACE))

#define PARSER_TOKEN_LOCATION(_parser, _idx) (&PARSER_CURSOR((_parser), (_idx))->source_location)

#define PARSER_SHIFT(_parser) (kefir_parser_token_cursor_next((_parser)->cursor))

#define REQUIRE_ALLOC(_ptr, _expr, _error)                                           \
    do {                                                                             \
        *(_ptr) = (_expr);                                                           \
        REQUIRE(*(_ptr) != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, (_error))); \
    } while (0)

#endif
