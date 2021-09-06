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

    kefir_result_t res;
    REQUIRE_MATCH_OK(
        &res, kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, external_declaration), NULL),
        KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(builder->parser, 0),
                               "Expected declaration or function definition"));
    REQUIRE_OK(kefir_parser_ast_builder_translation_unit(mem, builder));
    while (!PARSER_TOKEN_IS_SENTINEL(builder->parser, 0)) {
        REQUIRE_MATCH_OK(
            &res, kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, external_declaration), NULL),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(builder->parser, 0),
                                   "Expected declaration or function definition"));
        REQUIRE_OK(kefir_parser_ast_builder_translation_unit_append(mem, builder));
    }
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(translation_unit)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                             struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}
