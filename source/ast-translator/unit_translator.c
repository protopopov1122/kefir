/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Sloked project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/ast-translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_unit(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                        struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL && node->properties.category == KEFIR_AST_NODE_CATEGORY_TRANSLATION_UNIT &&
                node->klass->type == KEFIR_AST_TRANSLATION_UNIT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation unit"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));

    ASSIGN_DECL_CAST(struct kefir_ast_translation_unit *, unit, node->self);
    for (const struct kefir_list_entry *iter = kefir_list_head(&unit->external_definitions); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, external_definition, iter->value);

        switch (external_definition->properties.category) {
            case KEFIR_AST_NODE_CATEGORY_DECLARATION:
                // Intentionally left blank
                break;

            case KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION:
                REQUIRE_OK(kefir_ast_translate_function(mem, external_definition, context));
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected external definition node category");
        }
    }
    return KEFIR_OK;
}
