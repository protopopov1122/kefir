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
#include <stdio.h>
#include "kefir/ast/function_declaration_context.h"
#include "kefir/ast/context_impl.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/analyzer/initializer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/source_error.h"
#include "kefir/core/extensions.h"

static kefir_result_t context_resolve_scoped_ordinary_identifier(
    const struct kefir_ast_function_declaration_context *context, const char *identifier,
    const struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST scoped identifier pointer"));

    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = context->parent->resolve_ordinary_identifier(context->parent, identifier, scoped_identifier);
    } else if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

static kefir_result_t context_resolve_scoped_tag_identifier(
    const struct kefir_ast_function_declaration_context *context, const char *identifier,
    const struct kefir_ast_scoped_identifier **scoped_identifier) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST scoped identifier pointer"));

    struct kefir_ast_scoped_identifier *result = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->tag_scope, identifier, &result);
    if (res == KEFIR_NOT_FOUND) {
        res = context->parent->resolve_tag_identifier(context->parent, identifier, scoped_identifier);
    } else if (res == KEFIR_OK) {
        *scoped_identifier = result;
    }
    return res;
}

static kefir_result_t scoped_context_define_identifier(struct kefir_mem *mem,
                                                       struct kefir_ast_function_declaration_context *context,
                                                       const char *identifier, const struct kefir_ast_type *type,
                                                       kefir_ast_scoped_identifier_storage_t storage_class,
                                                       const struct kefir_source_location *location,
                                                       const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    const struct kefir_ast_type *adjusted_type =
        kefir_ast_type_conv_adjust_function_parameter(mem, context->context.type_bundle, type);
    REQUIRE(
        !KEFIR_AST_TYPE_IS_INCOMPLETE(adjusted_type),
        KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Identifier with no linkage shall have complete type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                      "Redeclaration of the same identifier with no linkage is not permitted");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, storage_class, NULL, KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE, false, NULL);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));

        const char *id = kefir_symbol_table_insert(mem, context->parent->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

static kefir_result_t scoped_context_define_constant(
    struct kefir_mem *mem, struct kefir_ast_function_declaration_context *context, const char *identifier,
    struct kefir_ast_constant_expression *value, const struct kefir_ast_type *type,
    const struct kefir_source_location *location, const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Constant cannot have variably-modified type"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Cannot redefine constant");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_constant(mem, value, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));

        const char *id = kefir_symbol_table_insert(mem, context->parent->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, id, scoped_id));
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

static kefir_result_t scoped_context_define_tag(struct kefir_mem *mem,
                                                struct kefir_ast_function_declaration_context *context,
                                                const struct kefir_ast_type *type,
                                                const struct kefir_source_location *location,
                                                const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(!kefir_ast_type_is_variably_modified(type),
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Type tag cannot refer to variably-modified type"));

    const char *identifier = NULL;
    REQUIRE_OK(kefir_ast_context_type_retrieve_tag(type, &identifier));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->tag_scope, identifier,
                                                            (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE_OK(kefir_ast_context_update_existing_scoped_type_tag(scoped_id, type));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_type_tag(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, context->parent->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->tag_scope, id, scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

static kefir_result_t scoped_context_declare_function(struct kefir_mem *mem,
                                                      struct kefir_ast_function_declaration_context *context,
                                                      kefir_ast_function_specifier_t specifier,
                                                      kefir_ast_scoped_identifier_storage_t storage_class,
                                                      const char *identifier, const struct kefir_ast_type *function,
                                                      const struct kefir_source_location *location,
                                                      const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST function type"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected function with non-empty identifier"));
    REQUIRE(!kefir_ast_type_is_variably_modified(function),
            KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Function type cannot be variably-modified"));

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;

    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location, "Cannot redefine function");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        struct kefir_ast_scoped_identifier *ordinary_id =
            kefir_ast_context_allocate_scoped_function_identifier(mem, function, specifier, storage_class, true);
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));

        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id));
        ASSIGN_PTR(scoped_id_ptr, ordinary_id);
    }
    return KEFIR_OK;
}

static kefir_result_t context_resolve_ordinary_identifier(const struct kefir_ast_context *context,
                                                          const char *identifier,
                                                          const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_function_declaration_context *, fn_ctx, context->payload);
    return context_resolve_scoped_ordinary_identifier(fn_ctx, identifier, scoped_id);
}

static kefir_result_t context_resolve_tag_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                     const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_function_declaration_context *, fn_ctx, context->payload);
    return context_resolve_scoped_tag_identifier(fn_ctx, identifier, scoped_id);
}

static kefir_result_t context_resolve_label_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                       const struct kefir_ast_scoped_identifier **scoped_id) {
    UNUSED(context);
    UNUSED(identifier);
    UNUSED(scoped_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Label resolving is not possible in the function declaration scope");
}

static kefir_result_t context_allocate_temporary_value(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_type *type,
                                                       struct kefir_ast_initializer *initializer,
                                                       const struct kefir_source_location *location,
                                                       struct kefir_ast_temporary_identifier *temp_id) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(type);
    UNUSED(initializer);
    UNUSED(location);
    UNUSED(temp_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "AST context does not support temporary values");
}

static kefir_result_t context_define_tag(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_type *type,
                                         const struct kefir_source_location *location) {
    UNUSED(location);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));

    ASSIGN_DECL_CAST(struct kefir_ast_function_declaration_context *, fn_ctx, context->payload);
    REQUIRE_OK(scoped_context_define_tag(mem, fn_ctx, type, location, NULL));
    return KEFIR_OK;
}

static kefir_result_t context_define_constant(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                              const char *identifier, struct kefir_ast_constant_expression *value,
                                              const struct kefir_ast_type *type,
                                              const struct kefir_source_location *location) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST constant expression"));

    ASSIGN_DECL_CAST(struct kefir_ast_function_declaration_context *, fn_ctx, context->payload);
    REQUIRE_OK(scoped_context_define_constant(mem, fn_ctx, identifier, value, type, location, NULL));
    return KEFIR_OK;
}

static kefir_result_t context_define_identifier(
    struct kefir_mem *mem, const struct kefir_ast_context *context, kefir_bool_t declaration, const char *identifier,
    const struct kefir_ast_type *type, kefir_ast_scoped_identifier_storage_t storage_class,
    kefir_ast_function_specifier_t function_specifier, struct kefir_ast_alignment *alignment,
    struct kefir_ast_initializer *initializer, const struct kefir_source_location *location,
    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type"));
    REQUIRE(alignment == NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Alignment specifier is not permitted in the declaration of function"));
    REQUIRE(declaration && initializer == NULL,
            KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                   "Initializer is not permitted in the declaration of function"));

    ASSIGN_DECL_CAST(struct kefir_ast_function_declaration_context *, fn_ctx, context->payload);

    kefir_bool_t is_function = type->tag == KEFIR_AST_TYPE_FUNCTION;
    if (is_function) {
        switch (storage_class) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
                REQUIRE(declaration, KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                                            "Function cannot be defined in local scope"));
                if (identifier != NULL) {
                    REQUIRE_OK(scoped_context_declare_function(mem, fn_ctx, function_specifier, storage_class,
                                                               identifier, type, location, scoped_id));
                }
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                              "Provided specifier is not permitted in declaration of function");
        }
    } else {
        REQUIRE(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                       "Function specifiers cannot be used for non-function types"));
        switch (storage_class) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                if (identifier != NULL) {
                    REQUIRE_OK(scoped_context_define_identifier(mem, fn_ctx, identifier, type, storage_class, location,
                                                                scoped_id));
                }
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                              "Provided specifier is not permitted in declaration of function");
        }
    }

    return KEFIR_OK;
}

static kefir_result_t context_reference_label(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                              const char *label, struct kefir_ast_flow_control_structure *parent,
                                              const struct kefir_source_location *location,
                                              const struct kefir_ast_scoped_identifier **scoped_id) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(label);
    UNUSED(parent);
    UNUSED(scoped_id);

    return KEFIR_SET_SOURCE_ERROR(KEFIR_ANALYSIS_ERROR, location,
                                  "Labels cannot be defined or referenced in a function declaration context");
}

static kefir_result_t context_push_block(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         struct kefir_ast_context_block_descriptor *block_descr) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(block_descr);

    return KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Blocks cannot be pushed in a function declaration context");
}

static kefir_result_t context_pop_block(struct kefir_mem *mem, const struct kefir_ast_context *context) {
    UNUSED(mem);
    UNUSED(context);

    return KEFIR_SET_ERROR(KEFIR_INVALID_CHANGE, "Blocks cannot be popped in a function declaration context");
}

kefir_result_t kefir_ast_function_declaration_context_init(struct kefir_mem *mem,
                                                           const struct kefir_ast_context *parent,
                                                           struct kefir_ast_function_declaration_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parent != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST global translation context"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));
    context->parent = parent;

    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->ordinary_scope,
                                                          kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->tag_scope, kefir_ast_context_free_scoped_identifier,
                                                          NULL));

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
    context->context.symbols = parent->symbols;
    context->context.type_bundle = parent->type_bundle;
    context->context.type_traits = parent->type_traits;
    context->context.target_env = parent->target_env;
    context->context.temporaries = NULL;
    context->context.type_analysis_context = KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER;
    context->context.flow_control_tree = NULL;
    context->context.global_context = parent->global_context;
    context->context.function_decl_contexts = parent->function_decl_contexts;
    context->context.configuration = parent->configuration;
    context->context.payload = context;

    context->context.extensions = parent->extensions;
    context->context.extensions_payload = NULL;
    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, &context->context, on_init);
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_function_declaration_context_free(struct kefir_mem *mem,
                                                           struct kefir_ast_function_declaration_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translatation context"));

    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, &context->context, on_free);
    REQUIRE_OK(res);
    context->context.extensions = NULL;
    context->context.extensions_payload = NULL;

    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->ordinary_scope));
    return KEFIR_OK;
}
