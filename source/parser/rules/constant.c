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
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(constant)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                     struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE(PARSER_TOKEN_IS(parser, 0, KEFIR_TOKEN_CONSTANT),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected constant token"));
    const struct kefir_token *token = PARSER_CURSOR(parser, 0);
    switch (token->constant.type) {
        case KEFIR_CONSTANT_TOKEN_INTEGER:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, token->constant.integer)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_LONG_INTEGER:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, token->constant.integer)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_LONG_LONG_INTEGER:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, token->constant.integer)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_INTEGER:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, token->constant.uinteger)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_INTEGER:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, token->constant.uinteger)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_LONG_INTEGER:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, token->constant.uinteger)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_FLOAT:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, token->constant.float32)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_DOUBLE:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, token->constant.float64)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_LONG_DOUBLE:
            REQUIRE_ALLOC(result,
                          KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_double(mem, token->constant.long_double)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_CHAR:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, token->constant.character)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_WIDE_CHAR:
            REQUIRE_ALLOC(result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_wide_char(mem, token->constant.wide_char)),
                          "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_UNICODE16_CHAR:
            REQUIRE_ALLOC(
                result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode16_char(mem, token->constant.unicode16_char)),
                "Failed to allocate AST constant");
            break;

        case KEFIR_CONSTANT_TOKEN_UNICODE32_CHAR:
            REQUIRE_ALLOC(
                result, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_unicode32_char(mem, token->constant.unicode32_char)),
                "Failed to allocate AST constant");
            break;
    }
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}
