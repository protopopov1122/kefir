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

#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_analyze_declaration_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_declaration *node,
                                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST declaration"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_DECLARATION;

    const struct kefir_ast_type *base_type = NULL;
    kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_ast_function_specifier_t function = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
    kefir_size_t alignment = 0;
    REQUIRE_OK(kefir_ast_analyze_declaration_specifiers(mem, context, &node->specifiers, &base_type, &storage,
                                                        &function, &alignment));

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->init_declarators); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, subnode, iter->value);
        REQUIRE(subnode->klass->type == KEFIR_AST_INIT_DECLARATOR,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &subnode->source_location,
                                       "Declaration list shall contain exclusively init declarators"));
        ASSIGN_DECL_CAST(struct kefir_ast_init_declarator *, init_decl, subnode->self);

        REQUIRE_OK(kefir_ast_analyze_init_declarator_node(mem, context, init_decl, subnode, base_type, storage,
                                                          function, alignment));
    }
    return KEFIR_OK;
}
