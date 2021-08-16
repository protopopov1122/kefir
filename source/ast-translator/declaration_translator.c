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

#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/initializer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_declaration(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                               struct kefir_irbuilder_block *builder,
                                               struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR builder block"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE((node->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR &&
             node->klass->type == KEFIR_AST_INIT_DECLARATOR) ||
                (node->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR &&
                 node->klass->type == KEFIR_AST_STATIC_ASSERTION) ||
                (node->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION &&
                 node->klass->type == KEFIR_AST_DECLARATION),
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected declaration AST node"));

    if (node->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR) {
        if (node->klass->type != KEFIR_AST_STATIC_ASSERTION) {
            ASSIGN_DECL_CAST(struct kefir_ast_init_declarator *, declaration, node->self);
            REQUIRE(declaration->initializer != NULL, KEFIR_OK);

            REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder,
                                                          declaration->base.properties.declaration_props.identifier,
                                                          declaration->base.properties.declaration_props.scoped_id));
            REQUIRE_OK(kefir_ast_translate_initializer(mem, context, builder, declaration->base.properties.type,
                                                       declaration->initializer));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    } else {
        ASSIGN_DECL_CAST(struct kefir_ast_declaration *, declaration, node->self);
        for (const struct kefir_list_entry *iter = kefir_list_head(&declaration->init_declarators); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl, iter->value);
            REQUIRE_OK(kefir_ast_translate_declaration(mem, decl, builder, context));
        }
    }
    return KEFIR_OK;
}
