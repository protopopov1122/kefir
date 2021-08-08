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

#include "kefir/ast/format.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_format_initializer_designation(struct kefir_json_output *json,
                                                        const struct kefir_ast_initializer_designation *designation) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid json output"));
    REQUIRE(designation != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer designation"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    if (designation->indexed) {
        REQUIRE_OK(kefir_json_output_string(json, "index"));
        REQUIRE_OK(kefir_json_output_object_key(json, "index"));
        REQUIRE_OK(kefir_ast_format(json, designation->index));
    } else {
        REQUIRE_OK(kefir_json_output_string(json, "member"));
        REQUIRE_OK(kefir_json_output_object_key(json, "member"));
        REQUIRE_OK(kefir_json_output_string(json, designation->identifier));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "next"));
    if (designation->next != NULL) {
        REQUIRE_OK(kefir_ast_format_initializer_designation(json, designation->next));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}
