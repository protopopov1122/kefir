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
#include "kefir/core/source_error.h"

static kefir_result_t scan_initializer(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_initializer **, initializer, payload);
    kefir_result_t res = kefir_parser_scan_initializer_list(mem, parser, initializer);
    if (res == KEFIR_NO_MATCH) {
        struct kefir_ast_node_base *node = NULL;
        REQUIRE_MATCH_OK(&res, KEFIR_PARSER_RULE_APPLY(mem, parser, assignment_expression, &node),
                         KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                                "Expected either initializer list, or assignment expression"));
        *initializer = kefir_ast_new_expression_initializer(mem, node);
        REQUIRE_ELSE(*initializer != NULL, {
            KEFIR_AST_NODE_FREE(mem, node);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST expression initializer");
        });
        (*initializer)->source_location = node->source_location;
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scan_initializer(struct kefir_mem *mem, struct kefir_parser *parser,
                                             struct kefir_ast_initializer **initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST initializer"));

    REQUIRE_OK(kefir_parser_try_invoke(mem, parser, scan_initializer, initializer));
    return KEFIR_OK;
}
