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
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast/downcast.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"

static kefir_result_t analyze_function_parameters(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_function_definition *node,
                                                  const struct kefir_ast_declarator_function *decl_func,
                                                  struct kefir_ast_local_context *local_context) {
    REQUIRE(
        kefir_list_length(&node->declarations) == 0,
        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                               "Function definition with non-empty parameter list shall not contain any declarations"));

    kefir_result_t res;
    for (const struct kefir_list_entry *iter = kefir_list_head(&decl_func->parameters); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, iter->value);
        REQUIRE(param->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION,
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected function parameter to be declaration"));

        struct kefir_ast_declaration *param_decl = NULL;
        REQUIRE_MATCH_OK(&res, kefir_ast_downcast_declaration(param, &param_decl),
                         KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &param->source_location,
                                                "Expected function parameter to be a declaration"));
        REQUIRE(kefir_list_length(&param_decl->init_declarators) == 1,
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected function parameter to have exactly one declarator"));

        ASSIGN_DECL_CAST(struct kefir_ast_init_declarator *, init_decl,
                         kefir_list_head(&param_decl->init_declarators)->value);
        const char *param_identifier = NULL;
        REQUIRE_OK(kefir_ast_declarator_unpack_identifier(init_decl->declarator, &param_identifier));
        REQUIRE(init_decl->base.properties.type != NULL,
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Function definition parameters shall have definite types"));
        if (init_decl->base.properties.type->tag != KEFIR_AST_TYPE_VOID && param_identifier != NULL) {
            const struct kefir_ast_type *adjusted_type = kefir_ast_type_conv_adjust_function_parameter(
                mem, context->type_bundle, init_decl->base.properties.type);

            REQUIRE_OK(kefir_ast_local_context_define_auto(mem, local_context, param_identifier, adjusted_type, NULL,
                                                           NULL, &init_decl->base.source_location, NULL));
        }
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_function_parameter_identifiers_impl(struct kefir_mem *mem,
                                                                  const struct kefir_ast_context *context,
                                                                  const struct kefir_ast_function_definition *node,
                                                                  const struct kefir_ast_declarator_function *decl_func,
                                                                  struct kefir_ast_local_context *local_context,
                                                                  const struct kefir_hashtree *argtree) {
    kefir_result_t res;
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->declarations); iter != NULL;
         kefir_list_next(&iter)) {

        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, decl_node, iter->value);
        struct kefir_ast_declaration *decl_list = NULL;
        struct kefir_ast_init_declarator *decl = NULL;
        REQUIRE_MATCH_OK(
            &res, kefir_ast_downcast_declaration(decl_node, &decl_list),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &decl_node->source_location,
                                   "Function definition declaration list shall contain exclusively declarations"));
        REQUIRE_OK(kefir_ast_declaration_unpack_single(decl_list, &decl));

        const char *identifier = NULL;
        const struct kefir_ast_type *type = NULL, *original_type = NULL;
        kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
        kefir_ast_function_specifier_t function_specifier = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
        kefir_size_t alignment = 0;
        REQUIRE_OK(kefir_ast_analyze_declaration(mem, &local_context->context, &decl->declaration->specifiers,
                                                 decl->declarator, &identifier, &original_type, &storage,
                                                 &function_specifier, &alignment));

        if (identifier != NULL) {
            identifier = kefir_symbol_table_insert(mem, context->symbols, identifier, NULL);
            REQUIRE(identifier != NULL,
                    KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert parameter identifier into symbol table"));
        }

        type = kefir_ast_type_conv_adjust_function_parameter(mem, context->type_bundle, original_type);

        REQUIRE(
            kefir_hashtree_has(argtree, (kefir_hashtree_key_t) identifier),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &decl_node->source_location,
                                   "Function definition declaration list declarations shall refer to identifier list"));
        REQUIRE(storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN ||
                    storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &decl_node->source_location,
                                       "Function definition declaration list shall not contain "
                                       "storage class specifiers other than register"));
        REQUIRE(alignment == 0,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &decl_node->source_location,
                                       "Function definition declaration list shall not contain alignment specifiers"));

        const struct kefir_ast_scoped_identifier *param_scoped_id = NULL;
        REQUIRE_OK(local_context->context.define_identifier(mem, &local_context->context, true, identifier, type,
                                                            storage, function_specifier, NULL, NULL,
                                                            &decl_node->source_location, &param_scoped_id));

        decl->base.properties.category = KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR;
        decl->base.properties.declaration_props.alignment = 0;
        decl->base.properties.declaration_props.function = KEFIR_AST_FUNCTION_SPECIFIER_NONE;
        decl->base.properties.declaration_props.identifier = identifier;
        decl->base.properties.declaration_props.scoped_id = param_scoped_id;
        decl->base.properties.declaration_props.static_assertion = false;
        decl->base.properties.declaration_props.storage = storage;
        decl->base.properties.type = original_type;

        decl_list->base.properties.category = KEFIR_AST_NODE_CATEGORY_DECLARATION;
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&decl_func->parameters); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, iter->value);
        REQUIRE(param->properties.type == NULL,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Function definition identifiers shall not have definite types"));

        struct kefir_ast_identifier *id_node = NULL;
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;
        REQUIRE_MATCH_OK(&res, kefir_ast_downcast_identifier(param, &id_node),
                         KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected parameter to be AST identifier"));
        REQUIRE_OK(local_context->context.resolve_ordinary_identifier(&local_context->context, id_node->identifier,
                                                                      &scoped_id));
        param->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
        param->properties.expression_props.identifier =
            kefir_symbol_table_insert(mem, context->symbols, id_node->identifier, NULL);
        REQUIRE(param->properties.expression_props.identifier != NULL,
                KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert parameter identifier into symbol table"));
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_function_parameter_identifiers(struct kefir_mem *mem,
                                                             const struct kefir_ast_context *context,
                                                             const struct kefir_ast_function_definition *node,
                                                             const struct kefir_ast_declarator_function *decl_func,
                                                             struct kefir_ast_local_context *local_context) {
    struct kefir_hashtree argtree;
    REQUIRE_OK(kefir_hashtree_init(&argtree, &kefir_hashtree_str_ops));

    kefir_result_t res = KEFIR_OK;
    for (const struct kefir_list_entry *iter = kefir_list_head(&decl_func->parameters); iter != NULL && res == KEFIR_OK;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, iter->value);
        struct kefir_ast_identifier *id_node = NULL;
        REQUIRE_MATCH(&res, kefir_ast_downcast_identifier(param, &id_node),
                      KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected parameter to be AST identifier"));
        REQUIRE_CHAIN(&res, kefir_hashtree_insert(mem, &argtree, (kefir_hashtree_key_t) id_node->identifier,
                                                  (kefir_hashtree_value_t) 0));
    }
    REQUIRE_CHAIN(&res,
                  analyze_function_parameter_identifiers_impl(mem, context, node, decl_func, local_context, &argtree));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &argtree);
        return res;
    });
    REQUIRE_OK(kefir_hashtree_free(mem, &argtree));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_function_definition_node(struct kefir_mem *mem,
                                                          const struct kefir_ast_context *context,
                                                          const struct kefir_ast_function_definition *node,
                                                          struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST function definition"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION;

    const struct kefir_ast_type *type = NULL;
    kefir_ast_scoped_identifier_storage_t storage = KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN;
    kefir_size_t alignment = 0;
    const char *function_identifier = NULL;
    REQUIRE_OK(kefir_ast_analyze_declaration(mem, context, &node->specifiers, node->declarator, &function_identifier,
                                             &type, &storage, &base->properties.function_definition.function,
                                             &alignment));
    REQUIRE_OK(kefir_ast_analyze_type(mem, context, context->type_analysis_context, type, &node->base.source_location));

    REQUIRE(type->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                   "Function definition declarator shall have function type"));
    REQUIRE(function_identifier != NULL, KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                                                "Function definition shall have non-empty identifier"));
    REQUIRE(alignment == 0, KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                                   "Function definition cannot have non-zero alignment"));
    base->properties.function_definition.identifier =
        kefir_symbol_table_insert(mem, context->symbols, function_identifier, NULL);
    REQUIRE(base->properties.function_definition.identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert function identifier into symbol table"));

    switch (storage) {
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                                          "Invalid function definition storage specifier");

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
            // Intentionally left blank
            break;
    }

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(context->define_identifier(mem, context, NULL, base->properties.function_definition.identifier, type,
                                          storage, base->properties.function_definition.function, NULL, NULL,
                                          &node->base.source_location, &scoped_id));
    REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Scoped identifier does not correspond to function definition type"));

    base->properties.type = scoped_id->function.type;
    base->properties.function_definition.storage = scoped_id->function.storage;
    base->properties.function_definition.scoped_id = scoped_id;

    struct kefir_ast_local_context *local_context = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_local_context));
    REQUIRE(local_context != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST local context"));

    kefir_result_t res = kefir_ast_local_context_init(mem, context->global_context, local_context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, local_context);
        return res;
    });
    local_context->context.surrounding_function = scoped_id;
    *scoped_id->function.local_context_ptr = local_context;

    REQUIRE_OK(kefir_ast_node_properties_init(&node->body->base.properties));
    node->body->base.properties.category = KEFIR_AST_NODE_CATEGORY_STATEMENT;

    REQUIRE(local_context->context.flow_control_tree != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                            "Expected function definition local context to have valid flow control tree"));
    REQUIRE_OK(kefir_ast_flow_control_tree_push(mem, local_context->context.flow_control_tree,
                                                KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK,
                                                &node->body->base.properties.statement_props.flow_control_statement));

    const struct kefir_ast_declarator_function *decl_func = NULL;
    REQUIRE_OK(kefir_ast_declarator_unpack_function(node->declarator, &decl_func));
    REQUIRE(decl_func != NULL,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected function definition to have function declarator"));
    switch (scoped_id->function.type->function_type.mode) {
        case KEFIR_AST_FUNCTION_TYPE_PARAMETERS:
            REQUIRE_OK(analyze_function_parameters(mem, context, node, decl_func, local_context));
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS:
            REQUIRE_OK(analyze_function_parameter_identifiers(mem, context, node, decl_func, local_context));
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY:
            REQUIRE(kefir_list_length(&node->declarations) == 0,
                    KEFIR_SET_SOURCE_ERROR(
                        KEFIR_ANALYSIS_ERROR, &node->base.source_location,
                        "Function definition with empty parameter list shall not contain any declarations"));
            break;
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->body->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, &local_context->context, item));
        REQUIRE(item->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT ||
                    item->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION ||
                    item->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, &item->source_location,
                                       "Compound statement items shall be either statements or declarations"));
    }
    REQUIRE_OK(kefir_ast_flow_control_tree_pop(local_context->context.flow_control_tree));
    return KEFIR_OK;
}
