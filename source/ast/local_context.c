/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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
#include <stdio.h>
#include "kefir/ast/local_context.h"
#include "kefir/ast/context_impl.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/initializer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include "kefir/core/extensions.h"

static kefir_result_t free_scoped_identifier(struct kefir_mem *mem, struct kefir_list *list,
                                             struct kefir_list_entry *entry, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid scoped identifier list entry"));
    UNUSED(list);
    ASSIGN_DECL_CAST(struct kefir_ast_scoped_identifier *, scoped_id, entry->value);
    return kefir_ast_context_free_scoped_identifier(mem, scoped_id, payload);
}

static kefir_result_t context_resolve_ordinary_identifier(const struct kefir_ast_context *context,
                                                          const char *identifier,
                                                          const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    return kefir_ast_local_context_resolve_scoped_ordinary_identifier(local_ctx, identifier, scoped_id);
}

static kefir_result_t context_resolve_tag_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                     const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    return kefir_ast_local_context_resolve_scoped_tag_identifier(local_ctx, identifier, scoped_id);
}

static kefir_result_t context_resolve_label_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                       const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid label"));
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST scoped identifier"));
    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);

    struct kefir_ast_scoped_identifier *label_id = NULL;
    REQUIRE_OK(kefir_ast_identifier_flat_scope_at(&local_ctx->label_scope, identifier, &label_id));
    *scoped_id = label_id;
    return KEFIR_OK;
}

static kefir_result_t define_temporaries(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                         const struct kefir_ast_type *type,
                                         const struct kefir_source_location *location,
                                         const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    REQUIRE(
        !KEFIR_AST_TYPE_IS_INCOMPLETE(type),
        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Identifier with no linkage shall have complete type"));

    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, root_scope, context->ordinary_scope.root.value);
    const char *identifier = KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER;

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(root_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                      "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, NULL, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE, false,
            NULL);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, root_scope, id, scoped_id));
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

static kefir_result_t context_allocate_temporary_value(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_type *type,
                                                       struct kefir_ast_initializer *initializer,
                                                       const struct kefir_source_location *location,
                                                       struct kefir_ast_temporary_identifier *temp_id) {
    UNUSED(location);
    UNUSED(initializer);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(temp_id != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid temporary identifier pointer"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    if (kefir_ast_temporaries_init(mem, context->type_bundle, true, context->temporaries)) {
        REQUIRE(context->temporaries->type != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to initialize a temporary type"));
        REQUIRE_OK(define_temporaries(mem, local_ctx, context->temporaries->type, location, NULL));
    }
    REQUIRE_OK(kefir_ast_temporaries_new_temporary(mem, context, type, temp_id));
    REQUIRE(temp_id->nested,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected AST local context temporaries to have nested structure"));
    return KEFIR_OK;
}

static kefir_result_t context_define_tag(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_type *type,
                                         const struct kefir_source_location *location) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    REQUIRE_OK(kefir_ast_local_context_define_tag(mem, local_ctx, type, location, NULL));
    return KEFIR_OK;
}

static kefir_result_t context_define_constant(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                              const char *identifier, struct kefir_ast_constant_expression *value,
                                              const struct kefir_ast_type *type,
                                              const struct kefir_source_location *location) {
    UNUSED(location);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    REQUIRE_OK(kefir_ast_local_context_define_constant(mem, local_ctx, identifier, value, type, location, NULL));
    return KEFIR_OK;
}

static kefir_result_t context_define_identifier(
    struct kefir_mem *mem, const struct kefir_ast_context *context, kefir_bool_t declaration, const char *identifier,
    const struct kefir_ast_type *type, kefir_ast_scoped_identifier_storage_t storage_class,
    kefir_ast_function_specifier_t function_specifier, struct kefir_ast_alignment *alignment,
    struct kefir_ast_initializer *initializer, const struct kefir_ast_declarator_attributes *attributes,
    const struct kefir_source_location *location, const struct kefir_ast_scoped_identifier **scoped_id) {

    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(identifier != NULL, KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                                       "Empty identifiers are not permitted in the local scope"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);

    const struct kefir_ast_type *unqualified_type = kefir_ast_unqualified_type(type);
    kefir_bool_t is_function = unqualified_type->tag == KEFIR_AST_TYPE_FUNCTION;
    if (is_function) {
        REQUIRE(alignment == NULL,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Alignment specifier is not permitted in the declaration of function"));
        REQUIRE(initializer == NULL,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Function cannot be defined with initializer"));
        switch (storage_class) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
                REQUIRE(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                               "Functin specifiers may only be used in function declarations"));
                REQUIRE(declaration,
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                               "Typedef specifier cannot be used for function definition"));
                REQUIRE_OK(kefir_ast_local_context_define_type(mem, local_ctx, identifier, unqualified_type, NULL,
                                                               location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
                REQUIRE(declaration, KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                                            "Function cannot be define in local scope"));
                REQUIRE_OK(kefir_ast_local_context_declare_function(
                    mem, local_ctx, function_specifier, storage_class == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
                    identifier, unqualified_type, attributes, location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                              "Illegal function storage-class specifier");
        }
    } else {
        REQUIRE(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Function specifiers cannot be used for non-function types"));
        REQUIRE(declaration || initializer != NULL,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Initializer must be provided to for identifier definition"));
        switch (storage_class) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
                REQUIRE_OK(kefir_ast_local_context_define_type(mem, local_ctx, identifier, type, alignment, location,
                                                               scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
                REQUIRE(declaration,
                        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                               "External identifier in block scope cannot have initializer"));
                REQUIRE_OK(kefir_ast_local_context_declare_external(mem, local_ctx, identifier, type, alignment,
                                                                    location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
                REQUIRE_OK(kefir_ast_local_context_define_static(mem, local_ctx, identifier, type, alignment,
                                                                 initializer, location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
                REQUIRE(declaration, KEFIR_SET_SOURCE_ERROR(
                                         KEFIR_ANALYSIS_ERROR, location,
                                         "External thread local identifier in block scope cannot have initializer"));
                REQUIRE_OK(kefir_ast_local_context_declare_external(mem, local_ctx, identifier, type, alignment,
                                                                    location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
                REQUIRE_OK(kefir_ast_local_context_define_static_thread_local(
                    mem, local_ctx, identifier, type, alignment, initializer, location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
                REQUIRE_OK(kefir_ast_local_context_define_auto(mem, local_ctx, identifier, type, alignment, initializer,
                                                               location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                REQUIRE_OK(kefir_ast_local_context_define_register(mem, local_ctx, identifier, type, alignment,
                                                                   initializer, location, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                              "Illegal identifier storage class in block scope");
        }
    }

    return KEFIR_OK;
}

kefir_result_t context_reference_label(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                       const char *label, struct kefir_ast_flow_control_structure *parent,
                                       const struct kefir_source_location *location,
                                       const struct kefir_ast_scoped_identifier **scoped_id) {
    UNUSED(location);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(label != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Labels are not permittedalid AST type"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    if (parent != NULL) {
        REQUIRE_OK(kefir_ast_local_context_define_label(mem, local_ctx, label, parent, location, scoped_id));
    } else {
        REQUIRE_OK(kefir_ast_local_context_reference_label(mem, local_ctx, label, scoped_id));
    }
    return KEFIR_OK;
}

static kefir_result_t context_push_block(struct kefir_mem *mem, const struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    REQUIRE_OK(kefir_ast_local_context_push_block_scope(mem, local_ctx));
    return KEFIR_OK;
}

static kefir_result_t context_pop_block(struct kefir_mem *mem, const struct kefir_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);
    REQUIRE_OK(kefir_ast_local_context_pop_block_scope(mem, local_ctx));
    return KEFIR_OK;
}

static kefir_result_t context_current_flow_control_point(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                         struct kefir_ast_flow_control_point **point) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(point != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to AST flow control point"));

    ASSIGN_DECL_CAST(struct kefir_ast_local_context *, local_ctx, context->payload);

    struct kefir_ast_flow_control_structure *current_flow_control_stmt = NULL;
    REQUIRE_OK(kefir_ast_flow_control_tree_top(context->flow_control_tree, &current_flow_control_stmt));

    *point = kefir_ast_flow_control_point_alloc(mem, current_flow_control_stmt);
    REQUIRE(*point != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST flow control point"));

    kefir_result_t res = kefir_list_insert_after(mem, &local_ctx->flow_control_points,
                                                 kefir_list_tail(&local_ctx->flow_control_points), *point);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_flow_control_point_free(mem, *point);
        *point = NULL;
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t free_flow_control_point(struct kefir_mem *mem, struct kefir_list *points,
                                              struct kefir_list_entry *entry, void *payload) {
    UNUSED(points);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_flow_control_point *, point, entry->value);

    REQUIRE_OK(kefir_ast_flow_control_point_free(mem, point));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_init(struct kefir_mem *mem, struct kefir_ast_global_context *global,
                                            struct kefir_ast_local_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST global translation context"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    context->global = global;
    context->temporaries = (struct kefir_ast_context_temporaries){0};
    REQUIRE_OK(kefir_list_init(&context->identifiers));
    REQUIRE_OK(kefir_list_on_remove(&context->identifiers, free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_init(mem, &context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_on_removal(&context->tag_scope,
                                                           kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->label_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->label_scope,
                                                          kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_flow_control_tree_init(&context->flow_control_tree));
    REQUIRE_OK(kefir_list_init(&context->flow_control_points));
    REQUIRE_OK(kefir_list_on_remove(&context->flow_control_points, free_flow_control_point, NULL));

    context->context.resolve_ordinary_identifier = context_resolve_ordinary_identifier;
    context->context.resolve_tag_identifier = context_resolve_tag_identifier;
    context->context.resolve_label_identifier = context_resolve_label_identifier;
    context->context.allocate_temporary_value = context_allocate_temporary_value;
    context->context.define_tag = context_define_tag;
    context->context.define_constant = context_define_constant;
    context->context.define_identifier = context_define_identifier;
    context->context.reference_label = context_reference_label;
    context->context.push_block = context_push_block;
    context->context.pop_block = context_pop_block;
    context->context.current_flow_control_point = context_current_flow_control_point;
    context->context.symbols = &context->global->symbols;
    context->context.type_bundle = &context->global->type_bundle;
    context->context.type_traits = context->global->type_traits;
    context->context.target_env = context->global->target_env;
    context->context.temporaries = &context->temporaries;
    context->context.type_analysis_context = KEFIR_AST_TYPE_ANALYSIS_DEFAULT;
    context->context.flow_control_tree = &context->flow_control_tree;
    context->context.global_context = global;
    context->context.function_decl_contexts = &global->function_decl_contexts;
    context->context.surrounding_function = NULL;
    context->context.configuration = global->context.configuration;
    context->context.payload = context;

    context->context.extensions = global->context.extensions;
    context->context.extensions_payload = NULL;
    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, &context->context, on_init);
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_free(struct kefir_mem *mem, struct kefir_ast_local_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));

    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, &context->context, on_free);
    REQUIRE_OK(res);
    context->context.extensions = NULL;
    context->context.extensions_payload = NULL;

    REQUIRE_OK(kefir_ast_flow_control_tree_free(mem, &context->flow_control_tree));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->label_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_free(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_list_free(mem, &context->identifiers));
    REQUIRE_OK(kefir_list_free(mem, &context->flow_control_points));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_resolve_scoped_ordinary_identifier(
    const struct kefir_ast_local_context *context, const char *identifier,
    const struct kefir_ast_scoped_identifier **scoped_identifier) {

    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->ordinary_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res =
            kefir_ast_global_context_resolve_scoped_ordinary_identifier(context->global, identifier, scoped_identifier);
    } else if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

kefir_result_t kefir_ast_local_context_resolve_scoped_tag_identifier(
    const struct kefir_ast_local_context *context, const char *identifier,
    const struct kefir_ast_scoped_identifier **scoped_identifier) {

    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_block_scope_at(&context->tag_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_global_context_resolve_scoped_tag_identifier(context->global, identifier, scoped_identifier);
    } else if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

kefir_result_t kefir_ast_local_context_push_block_scope(struct kefir_mem *mem,
                                                        struct kefir_ast_local_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));

    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_push(mem, &context->tag_scope));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_pop_block_scope(struct kefir_mem *mem, struct kefir_ast_local_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));

    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_block_scope_pop(&context->ordinary_scope));
    return KEFIR_OK;
}

static kefir_result_t require_global_ordinary_object(struct kefir_ast_global_context *context, const char *identifier,
                                                     kefir_bool_t thread_local, const struct kefir_ast_type *type,
                                                     const struct kefir_source_location *location,
                                                     struct kefir_ast_scoped_identifier **ordinary_id) {
    *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, ordinary_id);
    if (res == KEFIR_OK) {
        if (thread_local) {
            REQUIRE((*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL ||
                        (*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                           "Cannot redeclare identifier with different storage class"));
        } else {
            REQUIRE((*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
                        (*ordinary_id)->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                           "Cannot redeclare identifier with different storage class"));
        }
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, (*ordinary_id)->object.type, type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Cannot redeclare identifier with incompatible types"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_declare_external(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                        const char *identifier, const struct kefir_ast_type *type,
                                                        struct kefir_ast_alignment *alignment,
                                                        const struct kefir_source_location *location,
                                                        const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Variably-modified declaration in block scope cannot have extern specifier"));

    identifier = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));

    struct kefir_ast_scoped_identifier *global_ordinary_id = NULL, *ordinary_id = NULL;
    REQUIRE_OK(require_global_ordinary_object(context->global, identifier, false, type, location, &global_ordinary_id));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id == global_ordinary_id,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Local extern object identifier cannot be different than global"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "All declarations of the same identifier shall have compatible types"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(
            mem, &context->global->type_bundle, context->global->type_traits, ordinary_id->object.type, type);
        ASSIGN_PTR(scoped_id, ordinary_id);
    } else if (global_ordinary_id != NULL) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        REQUIRE_OK(
            kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, global_ordinary_id));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &global_ordinary_id->object.alignment, alignment));
        global_ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(
            mem, &context->global->type_bundle, context->global->type_traits, global_ordinary_id->object.type, type);
        ASSIGN_PTR(scoped_id, global_ordinary_id);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, true, NULL);
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res =
            kefir_ast_identifier_flat_scope_insert(mem, &context->global->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id));
        ASSIGN_PTR(scoped_id, ordinary_id);
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_declare_external_thread_local(
    struct kefir_mem *mem, struct kefir_ast_local_context *context, const char *identifier,
    const struct kefir_ast_type *type, struct kefir_ast_alignment *alignment,
    const struct kefir_source_location *location, const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type),
            KEFIR_SET_SOURCE_ERROR(
                KEFIR_ANALYSIS_ERROR, location,
                "Variably-modified declaration in block scope cannot have extern _Thread_local specifiers"));

    identifier = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
    REQUIRE(identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));

    struct kefir_ast_scoped_identifier *global_ordinary_id = NULL, *ordinary_id = NULL;
    REQUIRE_OK(require_global_ordinary_object(context->global, identifier, true, type, location, &global_ordinary_id));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id == global_ordinary_id,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Local extern object identifier cannot be different than global"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "All declarations of the same identifier shall have compatible types"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(
            mem, &context->global->type_bundle, context->global->type_traits, ordinary_id->object.type, type);
        ASSIGN_PTR(scoped_id, ordinary_id);
    } else if (global_ordinary_id != NULL) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        REQUIRE_OK(
            kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, global_ordinary_id));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &global_ordinary_id->object.alignment, alignment));
        global_ordinary_id->object.type = KEFIR_AST_TYPE_COMPOSITE(
            mem, &context->global->type_bundle, context->global->type_traits, global_ordinary_id->object.type, type);
        ASSIGN_PTR(scoped_id, global_ordinary_id);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, true, NULL);
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res =
            kefir_ast_identifier_flat_scope_insert(mem, &context->global->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id));
        ASSIGN_PTR(scoped_id, ordinary_id);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_static(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                     const char *identifier, const struct kefir_ast_type *type,
                                                     struct kefir_ast_alignment *alignment,
                                                     struct kefir_ast_initializer *initializer,
                                                     const struct kefir_source_location *location,
                                                     const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type) ||
                kefir_ast_unqualified_type(type)->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
            KEFIR_SET_SOURCE_ERROR(
                KEFIR_ANALYSIS_ERROR, location,
                "Variably-modified declaration in block scope with static specifier shall be a pointer"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type) || initializer == NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Variably-modified declaration cannot have initializer"));

    if (initializer == NULL) {
        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
    }

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                      "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, alignment, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE,
            false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->global->type_bundle, type, qualifications);
        }

        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
        scoped_id->type = type;
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_static_thread_local(
    struct kefir_mem *mem, struct kefir_ast_local_context *context, const char *identifier,
    const struct kefir_ast_type *type, struct kefir_ast_alignment *alignment, struct kefir_ast_initializer *initializer,
    const struct kefir_source_location *location, const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(
        !kefir_ast_type_is_variably_modified(type) ||
            kefir_ast_unqualified_type(type)->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
        KEFIR_SET_SOURCE_ERROR(
            KEFIR_ANALYSIS_ERROR, location,
            "Variably-modified declaration in block scope with static _Thread_local specifiers shall be a pointer"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type) || initializer == NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Variably-modified declaration cannot have initializer"));

    if (initializer == NULL) {
        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
    }

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                      "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE, false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->global->type_bundle, type, qualifications);
        }

        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
        scoped_id->type = type;
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

static kefir_result_t find_block_structure(const struct kefir_ast_flow_control_structure *stmt, void *payload,
                                           kefir_bool_t *result) {
    UNUSED(payload);
    REQUIRE(stmt != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control structure"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to result"));

    *result = stmt->type == KEFIR_AST_FLOW_CONTROL_STRUCTURE_BLOCK;
    return KEFIR_OK;
}

static kefir_result_t register_vla(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                   struct kefir_ast_scoped_identifier *scoped_id) {

    struct kefir_ast_flow_control_structure *block = NULL;
    kefir_result_t res =
        kefir_ast_flow_control_tree_traverse(&context->flow_control_tree, find_block_structure, NULL, &block);

    if (res != KEFIR_NOT_FOUND && block != NULL) {
        REQUIRE_OK(res);
        block->value.block.contains_vla = true;

        struct kefir_ast_flow_control_data_element *data_elt =
            kefir_ast_flow_control_data_element_alloc(mem, context->flow_control_tree.data_element_track.vla_id++,
                                                      block, KEFIR_AST_FLOW_CONTROL_DATA_ELEMENT_VLA);
        REQUIRE(data_elt != NULL,
                KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate AST flow control data element"));

        res = kefir_ast_flow_control_block_add_data_element(mem, block, data_elt);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_flow_control_data_element_free(mem, data_elt);
            return res;
        });

        scoped_id->object.data_element = data_elt;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_auto(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                   const char *identifier, const struct kefir_ast_type *type,
                                                   struct kefir_ast_alignment *alignment,
                                                   struct kefir_ast_initializer *initializer,
                                                   const struct kefir_source_location *location,
                                                   const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type) || initializer == NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Variably-modified declaration cannot have initializer"));

    if (initializer == NULL) {
        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
    }

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                      "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO, alignment, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE,
            false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->global->type_bundle, type, qualifications);
        }
        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
        scoped_id->type = type;
    }

    if (KEFIR_AST_TYPE_IS_VL_ARRAY(type)) {
        REQUIRE_OK(register_vla(mem, context, scoped_id));
    }

    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_register(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                       const char *identifier, const struct kefir_ast_type *type,
                                                       struct kefir_ast_alignment *alignment,
                                                       struct kefir_ast_initializer *initializer,
                                                       const struct kefir_source_location *location,
                                                       const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type) || initializer == NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Variably-modified declaration cannot have initializer"));

    if (initializer == NULL) {
        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
    }

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                      "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER, alignment, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE,
            false, initializer);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->global->type_bundle, type, qualifications);
        }
        REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Identifier with no linkage shall have complete type"));
        scoped_id->type = type;
    }

    if (KEFIR_AST_TYPE_IS_VL_ARRAY(type)) {
        REQUIRE_OK(register_vla(mem, context, scoped_id));
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_constant(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                       const char *identifier,
                                                       struct kefir_ast_constant_expression *value,
                                                       const struct kefir_ast_type *type,
                                                       const struct kefir_source_location *location,
                                                       const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Constant definition cannot have variably-modified type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Cannot redefine constant");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_constant(mem, value, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_tag(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                  const struct kefir_ast_type *type,
                                                  const struct kefir_source_location *location,
                                                  const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    UNUSED(location);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Type tag definition cannot have variably-modified type"));

    const char *identifier = NULL;
    REQUIRE_OK(kefir_ast_context_type_retrieve_tag(type, &identifier));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res =
        kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->tag_scope), identifier,
                                           (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE_OK(kefir_ast_context_update_existing_scoped_type_tag(scoped_id, type));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_type_tag(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        res = kefir_ast_identifier_block_scope_insert(mem, &context->tag_scope, id, scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_type(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                   const char *identifier, const struct kefir_ast_type *type,
                                                   struct kefir_ast_alignment *alignment,
                                                   const struct kefir_source_location *location,
                                                   const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res =
        kefir_ast_identifier_flat_scope_at(kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier,
                                           (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION &&
                    KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, scoped_id->type_definition.type, type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Unable to redefine different type with the same identifier"));
        if (KEFIR_AST_TYPE_IS_INCOMPLETE(scoped_id->type_definition.type) && !KEFIR_AST_TYPE_IS_INCOMPLETE(type)) {
            scoped_id->type = type;
        }
        REQUIRE_OK(kefir_ast_context_allocate_scoped_type_definition_update_alignment(mem, scoped_id, alignment));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_type_definition(mem, type, alignment);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_list_insert_after(mem, &context->identifiers, kefir_list_tail(&context->identifiers), scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        const char *id = kefir_symbol_table_insert(mem, &context->global->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

static kefir_result_t require_global_ordinary_function(struct kefir_ast_global_context *context, const char *identifier,
                                                       const struct kefir_ast_type *type,
                                                       const struct kefir_source_location *location,
                                                       struct kefir_ast_scoped_identifier **ordinary_id) {
    *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->function_identifiers, identifier, ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, (*ordinary_id)->object.type, type),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Cannot redeclare identifier with incompatible types"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_declare_function(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                        kefir_ast_function_specifier_t specifier,
                                                        kefir_bool_t external_visibility, const char *identifier,
                                                        const struct kefir_ast_type *function,
                                                        const struct kefir_ast_declarator_attributes *attributes,
                                                        const struct kefir_source_location *location,
                                                        const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST function type"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Expected function with non-empty identifier"));
    REQUIRE(!kefir_ast_type_is_variably_modified(function),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Function type cannot be variably-modified"));

    struct kefir_ast_scoped_identifier *global_ordinary_id = NULL, *ordinary_id = NULL;
    REQUIRE_OK(require_global_ordinary_function(context->global, identifier, function, location, &global_ordinary_id));

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(
        kefir_ast_identifier_block_scope_top(&context->ordinary_scope), identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id == global_ordinary_id,
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Local extern function identifier cannot be different than global"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->global->type_traits, ordinary_id->function.type, function),
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "All declarations of the same identifier shall have compatible types"));
        ordinary_id->function.type = KEFIR_AST_TYPE_COMPOSITE(
            mem, &context->global->type_bundle, context->global->type_traits, ordinary_id->function.type, function);
        ordinary_id->function.specifier =
            kefir_ast_context_merge_function_specifiers(ordinary_id->function.specifier, specifier);
        ordinary_id->function.inline_definition = ordinary_id->function.inline_definition && !external_visibility &&
                                                  kefir_ast_function_specifier_is_inline(specifier);
        if (attributes != NULL) {
            ordinary_id->function.flags.gnu_inline = ordinary_id->function.flags.gnu_inline || attributes->gnu_inline;
        }
        ASSIGN_PTR(scoped_id_ptr, ordinary_id);
    } else if (global_ordinary_id != NULL) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        REQUIRE_OK(
            kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, global_ordinary_id));
        global_ordinary_id->function.type =
            KEFIR_AST_TYPE_COMPOSITE(mem, &context->global->type_bundle, context->global->type_traits,
                                     global_ordinary_id->function.type, function);
        global_ordinary_id->function.specifier =
            kefir_ast_context_merge_function_specifiers(global_ordinary_id->function.specifier, specifier);
        global_ordinary_id->function.inline_definition = global_ordinary_id->function.inline_definition &&
                                                         !external_visibility &&
                                                         kefir_ast_function_specifier_is_inline(specifier);
        if (attributes != NULL) {
            global_ordinary_id->function.flags.gnu_inline =
                global_ordinary_id->function.flags.gnu_inline || attributes->gnu_inline;
        }
        ASSIGN_PTR(scoped_id_ptr, global_ordinary_id);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *ordinary_id = kefir_ast_context_allocate_scoped_function_identifier(
            mem, function, specifier, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, true, false,
            !external_visibility && kefir_ast_function_specifier_is_inline(specifier));
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocte AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->global->function_identifiers, identifier,
                                                     ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
        REQUIRE_OK(kefir_ast_identifier_block_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id));
        ordinary_id->function.flags.gnu_inline = attributes != NULL && attributes->gnu_inline;
        ASSIGN_PTR(scoped_id_ptr, ordinary_id);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_reference_label(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                       const char *label,
                                                       const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST local context"));
    REQUIRE(label != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid label"));

    label = kefir_symbol_table_insert(mem, context->context.symbols, label, NULL);
    REQUIRE(label != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate label"));

    struct kefir_ast_scoped_identifier *label_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->label_scope, label, &label_id);
    if (res == KEFIR_NOT_FOUND) {
        label_id = kefir_ast_context_allocate_scoped_label(mem, NULL);
        REQUIRE(label_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->label_scope, label, label_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, label_id, NULL);
            return res;
        });
    } else {
        REQUIRE_OK(res);
    }

    ASSIGN_PTR(scoped_id, label_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_context_define_label(struct kefir_mem *mem, struct kefir_ast_local_context *context,
                                                    const char *label, struct kefir_ast_flow_control_structure *parent,
                                                    const struct kefir_source_location *location,
                                                    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST local context"));
    REQUIRE(label != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid label"));
    REQUIRE(parent != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST flow control structure"));

    label = kefir_symbol_table_insert(mem, context->context.symbols, label, NULL);
    REQUIRE(label != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate label"));

    struct kefir_ast_scoped_identifier *label_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->label_scope, label, &label_id);
    if (res == KEFIR_NOT_FOUND) {
        label_id = kefir_ast_context_allocate_scoped_label(mem, parent);
        REQUIRE(label_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->label_scope, label, label_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, label_id, NULL);
            return res;
        });
    } else {
        REQUIRE_OK(res);
        REQUIRE(label_id->label.point != NULL && label_id->label.point->parent == NULL,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Cannot redefine a label"));
        label_id->label.point->parent = parent;
        REQUIRE_OK(kefir_ast_flow_control_point_bound(label_id->label.point));
    }

    ASSIGN_PTR(scoped_id, label_id);
    return KEFIR_OK;
}
