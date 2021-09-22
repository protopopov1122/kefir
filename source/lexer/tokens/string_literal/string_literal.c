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

#include "kefir/lexer/lexer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct params {
    struct kefir_token *token;
    kefir_bool_t merge_adjacent;
};

static kefir_result_t match_impl(struct kefir_mem *mem, struct kefir_lexer *lexer, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct params *, params, payload);

    kefir_result_t res = kefir_lexer_next_narrow_string_literal(mem, lexer, params->token, params->merge_adjacent);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = kefir_lexer_next_unicode8_string_literal(mem, lexer, params->token, params->merge_adjacent);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = kefir_lexer_next_unicode16_string_literal(mem, lexer, params->token, params->merge_adjacent);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = kefir_lexer_next_unicode32_string_literal(mem, lexer, params->token, params->merge_adjacent);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = kefir_lexer_next_wide_string_literal(mem, lexer, params->token, params->merge_adjacent);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match string literal");
}

kefir_result_t kefir_lexer_match_string_literal(struct kefir_mem *mem, struct kefir_lexer *lexer,
                                                struct kefir_token *token, kefir_bool_t merge_adjacent) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));
    REQUIRE(token != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token"));

    struct params params = {.token = token, .merge_adjacent = merge_adjacent};
    REQUIRE_OK(kefir_lexer_apply(mem, lexer, match_impl, &params));
    return KEFIR_OK;
}
