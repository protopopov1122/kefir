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
#include "kefir/ast-translator/misc.h"
#include "kefir/ast/downcast.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t translate_vla_declaration(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                                struct kefir_irbuilder_block *builder,
                                                struct kefir_ast_translator_context *context) {
    REQUIRE(node->properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO ||
                node->properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE,
                            "Variable-length array can only have either automatic or register storage specifier"));

    ASSIGN_DECL_CAST(struct kefir_ast_init_declarator *, declaration, node->self);
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                     declaration->base.properties.declaration_props.scoped_id->payload.ptr);

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_GETLOCALS, 0));
    REQUIRE_OK(
        kefir_ast_translator_resolve_type_layout(builder, identifier_data->type_id, identifier_data->layout, NULL));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, identifier_data->type_id,
                                               identifier_data->layout->vl_array.array_ptr_field));
    REQUIRE_OK(kefir_ast_translate_sizeof(mem, context, builder, node->properties.type));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_GETLOCALS, 0));
    REQUIRE_OK(
        kefir_ast_translator_resolve_type_layout(builder, identifier_data->type_id, identifier_data->layout, NULL));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, identifier_data->type_id,
                                               identifier_data->layout->vl_array.array_size_field));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_PICK, 1));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE64, 0));
    REQUIRE_OK(kefir_ast_translate_alignof(mem, context, builder, node->properties.type));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_ALLOCA, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE64, 0));
    return KEFIR_OK;
}

struct vl_modified_param {
    struct kefir_mem *mem;
    struct kefir_irbuilder_block *builder;
    struct kefir_ast_translator_context *context;
};

static kefir_result_t translate_variably_modified(const struct kefir_ast_node_base *node, void *payload) {
    REQUIRE(node != NULL, KEFIR_OK);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct vl_modified_param *, param, payload);

    REQUIRE_OK(kefir_ast_translate_expression(param->mem, node, param->builder, param->context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(param->builder, KEFIR_IROPCODE_POP, 0));
    return KEFIR_OK;
}

static kefir_result_t translate_init_declarator(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                                struct kefir_irbuilder_block *builder,
                                                struct kefir_ast_translator_context *context) {
    REQUIRE(node->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected init declarator node"));
    if (node->properties.declaration_props.static_assertion ||
        node->properties.declaration_props.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF) {
        // Deliberately left blank
    } else if (KEFIR_AST_TYPE_IS_VL_ARRAY(node->properties.type)) {
        REQUIRE_OK(translate_vla_declaration(mem, node, builder, context));
    } else {
        struct vl_modified_param vl_param = {.mem = mem, .context = context, .builder = builder};
        REQUIRE_OK(
            kefir_ast_type_list_variable_modificators(node->properties.type, translate_variably_modified, &vl_param));

        struct kefir_ast_init_declarator *init_decl = NULL;
        REQUIRE_OK(kefir_ast_downcast_init_declarator(node, &init_decl));
        REQUIRE(init_decl->initializer != NULL, KEFIR_OK);

        kefir_ast_scoped_identifier_storage_t storage = node->properties.declaration_props.storage;
        REQUIRE(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO ||
                    storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                KEFIR_OK);

        REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder,
                                                      node->properties.declaration_props.identifier,
                                                      node->properties.declaration_props.scoped_id));
        REQUIRE_OK(
            kefir_ast_translate_initializer(mem, context, builder, node->properties.type, init_decl->initializer));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_declaration(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                               struct kefir_irbuilder_block *builder,
                                               struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR builder block"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator context"));
    REQUIRE(node->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected declaration AST node"));

    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, declaration, node->self);
    for (const struct kefir_list_entry *iter = kefir_list_head(&declaration->init_declarators); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl, iter->value);
        REQUIRE_OK(translate_init_declarator(mem, decl, builder, context));
    }
    return KEFIR_OK;
}
