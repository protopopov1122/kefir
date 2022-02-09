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

#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/layout.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast-translator/scope/scope_layout_impl.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast/type_completion.h"
#include "kefir/core/source_error.h"

kefir_result_t kefir_ast_translator_local_scope_layout_init(struct kefir_mem *mem, struct kefir_ir_module *module,
                                                            struct kefir_ast_translator_global_scope_layout *global,
                                                            struct kefir_ast_translator_local_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST global scope layout"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST local scope layout"));
    layout->local_context = NULL;
    REQUIRE_OK(kefir_list_init(&layout->local_objects));
    REQUIRE_OK(kefir_list_init(&layout->static_objects));
    REQUIRE_OK(kefir_list_init(&layout->static_thread_local_objects));
    REQUIRE_OK(kefir_list_on_remove(&layout->local_objects, kefir_ast_translator_scoped_identifier_remove, NULL));
    REQUIRE_OK(kefir_list_on_remove(&layout->static_objects, kefir_ast_translator_scoped_identifier_remove, NULL));
    REQUIRE_OK(kefir_list_on_remove(&layout->static_thread_local_objects, kefir_ast_translator_scoped_identifier_remove,
                                    NULL));
    layout->global = global;
    layout->local_layout = kefir_ir_module_new_type(mem, module, 0, &layout->local_layout_id);
    layout->local_type_layout = NULL;
    REQUIRE(layout->local_layout != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate new IR type"));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_local_scope_layout_free(struct kefir_mem *mem,
                                                            struct kefir_ast_translator_local_scope_layout *layout) {
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST local scope layout"));
    if (layout->local_context != NULL) {
        REQUIRE_OK(kefir_ast_identifier_block_scope_cleanup_payload(mem, &layout->local_context->ordinary_scope));
        layout->local_context = NULL;
    }
    if (layout->local_type_layout != NULL) {
        REQUIRE_OK(kefir_ast_type_layout_free(mem, layout->local_type_layout));
        layout->local_type_layout = NULL;
    }
    REQUIRE_OK(kefir_list_free(mem, &layout->local_objects));
    REQUIRE_OK(kefir_list_free(mem, &layout->static_objects));
    REQUIRE_OK(kefir_list_free(mem, &layout->static_thread_local_objects));
    layout->global = NULL;
    layout->local_layout = NULL;
    return KEFIR_OK;
}

static kefir_result_t translate_static_identifier(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_translator_environment *env,
                                                  struct kefir_ast_translator_local_scope_layout *local_layout,
                                                  const char *identifier,
                                                  const struct kefir_ast_scoped_identifier *scoped_identifier) {
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_layout,
                     scoped_identifier->payload.ptr);
    KEFIR_AST_SCOPE_SET_CLEANUP(scoped_identifier, kefir_ast_translator_scoped_identifer_payload_free, NULL);
    struct kefir_irbuilder_type global_builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &global_builder, local_layout->global->static_layout));

    const struct kefir_ast_type *object_type = NULL;
    kefir_result_t res = kefir_ast_type_completion(mem, context, &object_type, scoped_identifier->object.type);
    REQUIRE_CHAIN(&res, kefir_ast_translate_object_type(mem, object_type, scoped_identifier->object.alignment->value,
                                                        env, &global_builder, &scoped_identifier_layout->layout));

    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&global_builder);
        return res;
    });
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&global_builder));
    scoped_identifier_layout->type_id = local_layout->global->static_layout_id;
    scoped_identifier_layout->type = local_layout->global->static_layout;

    REQUIRE_OK(kefir_ast_translator_evaluate_type_layout(mem, env, scoped_identifier_layout->layout,
                                                         scoped_identifier_layout->type));
    REQUIRE_OK(kefir_ast_translator_scoped_identifier_insert(mem, identifier, scoped_identifier,
                                                             &local_layout->static_objects));
    return KEFIR_OK;
}

static kefir_result_t translate_static_thread_local_identifier(
    struct kefir_mem *mem, const struct kefir_ast_translator_environment *env,
    struct kefir_ast_translator_local_scope_layout *local_layout, const char *identifier,
    const struct kefir_ast_scoped_identifier *scoped_identifier) {
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_layout,
                     scoped_identifier->payload.ptr);
    KEFIR_AST_SCOPE_SET_CLEANUP(scoped_identifier, kefir_ast_translator_scoped_identifer_payload_free, NULL);
    struct kefir_irbuilder_type global_builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &global_builder, local_layout->global->static_thread_local_layout));
    kefir_result_t res =
        kefir_ast_translate_object_type(mem, scoped_identifier->object.type, scoped_identifier->object.alignment->value,
                                        env, &global_builder, &scoped_identifier_layout->layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&global_builder);
        return res;
    });
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&global_builder));
    scoped_identifier_layout->type_id = local_layout->global->static_thread_local_layout_id;
    scoped_identifier_layout->type = local_layout->global->static_thread_local_layout;

    REQUIRE_OK(kefir_ast_translator_evaluate_type_layout(mem, env, scoped_identifier_layout->layout,
                                                         scoped_identifier_layout->type));
    REQUIRE_OK(kefir_ast_translator_scoped_identifier_insert(mem, identifier, scoped_identifier,
                                                             &local_layout->static_thread_local_objects));
    return KEFIR_OK;
}

static kefir_result_t translate_auto_register_identifier(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                         struct kefir_irbuilder_type *builder,
                                                         const struct kefir_ast_translator_environment *env,
                                                         struct kefir_ast_translator_local_scope_layout *local_layout,
                                                         const char *identifier,
                                                         const struct kefir_ast_scoped_identifier *scoped_identifier,
                                                         struct kefir_ast_type_layout *scope_type_layout) {
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_layout,
                     scoped_identifier->payload.ptr);
    KEFIR_AST_SCOPE_SET_CLEANUP(scoped_identifier, kefir_ast_translator_scoped_identifer_payload_free, NULL);

    const struct kefir_ast_type *object_type = NULL;
    REQUIRE_OK(kefir_ast_type_completion(mem, context, &object_type, scoped_identifier->object.type));

    REQUIRE_OK(kefir_ast_translate_object_type(mem, object_type, scoped_identifier->object.alignment->value, env,
                                               builder, &scoped_identifier_layout->layout));
    scoped_identifier_layout->type_id = local_layout->local_layout_id;
    scoped_identifier_layout->type = builder->type;
    REQUIRE_OK(kefir_list_insert_after(mem, &scope_type_layout->custom_layout.sublayouts,
                                       kefir_list_tail(&scope_type_layout->custom_layout.sublayouts),
                                       scoped_identifier_layout->layout));
    scoped_identifier_layout->layout->parent = scope_type_layout;

    REQUIRE_OK(kefir_ast_translator_evaluate_type_layout(mem, env, scoped_identifier_layout->layout,
                                                         scoped_identifier_layout->type));
    REQUIRE_OK(kefir_ast_translator_scoped_identifier_insert(mem, identifier, scoped_identifier,
                                                             &local_layout->local_objects));
    return KEFIR_OK;
}

static kefir_result_t translate_local_scoped_identifier_object(
    struct kefir_mem *mem, const struct kefir_ast_context *context, struct kefir_irbuilder_type *builder,
    const char *identifier, const struct kefir_ast_scoped_identifier *scoped_identifier,
    const struct kefir_ast_translator_environment *env, struct kefir_ast_translator_local_scope_layout *local_layout,
    struct kefir_ast_type_layout *scope_type_layout, struct kefir_ir_typeentry *wrapper_structure) {
    REQUIRE(scoped_identifier->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT, KEFIR_OK);
    switch (scoped_identifier->object.storage) {
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
            REQUIRE_OK(translate_static_identifier(mem, context, env, local_layout, identifier, scoped_identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST,
                                   "Cannot have thread local block-scope variable with no linkage");

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
            REQUIRE_OK(translate_static_thread_local_identifier(mem, env, local_layout, identifier, scoped_identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            wrapper_structure->param++;
            REQUIRE_OK(translate_auto_register_identifier(mem, context, builder, env, local_layout, identifier,
                                                          scoped_identifier, scope_type_layout));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
            return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unexpected storage class of local-scope variable");
    }
    return KEFIR_OK;
}

static kefir_result_t translate_local_scoped_identifier_function(
    struct kefir_mem *mem, const char *identifier, const struct kefir_ast_scoped_identifier *scoped_identifier,
    const struct kefir_ast_translator_environment *env, struct kefir_ast_type_bundle *type_bundle,
    const struct kefir_ast_type_traits *type_traits, struct kefir_ir_module *module) {
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_function *, scoped_identifier_func,
                     scoped_identifier->payload.ptr);
    KEFIR_AST_SCOPE_SET_CLEANUP(scoped_identifier, kefir_ast_translator_scoped_identifer_payload_free, NULL);
    REQUIRE_OK(kefir_ast_translator_function_declaration_init(mem, env, type_bundle, type_traits, module, NULL,
                                                              identifier, scoped_identifier->function.type, NULL,
                                                              &scoped_identifier_func->declaration));
    return KEFIR_OK;
}

static kefir_result_t translate_local_scoped_identifier(
    struct kefir_mem *mem, const struct kefir_ast_context *context, struct kefir_irbuilder_type *builder,
    const char *identifier, const struct kefir_ast_scoped_identifier *scoped_identifier,
    const struct kefir_ast_translator_environment *env, struct kefir_ast_type_bundle *type_bundle,
    const struct kefir_ast_type_traits *type_traits, struct kefir_ir_module *module,
    struct kefir_ast_translator_local_scope_layout *local_layout, struct kefir_ast_type_layout *scope_type_layout,
    struct kefir_ir_typeentry *wrapper_structure) {
    switch (scoped_identifier->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
            REQUIRE_OK(translate_local_scoped_identifier_object(mem, context, builder, identifier, scoped_identifier,
                                                                env, local_layout, scope_type_layout,
                                                                wrapper_structure));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
            REQUIRE_OK(translate_local_scoped_identifier_function(mem, identifier, scoped_identifier, env, type_bundle,
                                                                  type_traits, module));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
            REQUIRE(scoped_identifier->label.point != NULL && scoped_identifier->label.point->parent != NULL,
                    KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Cannot translate undefined label"));
            REQUIRE_OK(kefir_ast_translator_flow_control_point_init(mem, scoped_identifier->label.point, NULL));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t local_scope_empty(struct kefir_mem *mem, const struct kefir_tree_node *root,
                                        kefir_bool_t *empty) {
    *empty = true;
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, scope, root->value);
    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res;
    for (res = kefir_ast_identifier_flat_scope_iter(scope, &iter); res == KEFIR_OK;
         res = kefir_ast_identifier_flat_scope_next(scope, &iter)) {
        if (iter.value->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT) {
            if (iter.value->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO ||
                iter.value->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER) {
                *empty = false;
                return KEFIR_OK;
            }
        } else if (iter.value->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL) {
            *empty = false;
            return KEFIR_OK;
        }
    }
    REQUIRE(res == KEFIR_ITERATOR_END, res);
    if (kefir_tree_first_child(root) != NULL) {
        for (struct kefir_tree_node *child = kefir_tree_first_child(root); child != NULL;
             child = kefir_tree_next_sibling(child)) {
            REQUIRE_OK(local_scope_empty(mem, child, empty));
            REQUIRE(*empty, KEFIR_OK);
        }
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_local_scope(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                           const struct kefir_tree_node *root, struct kefir_irbuilder_type *builder,
                                           const struct kefir_ast_translator_environment *env,
                                           struct kefir_ast_type_bundle *type_bundle,
                                           const struct kefir_ast_type_traits *type_traits,
                                           struct kefir_ir_module *module,
                                           struct kefir_ast_translator_local_scope_layout *local_layout,
                                           struct kefir_ast_type_layout **scope_type_layout) {
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, scope, root->value);
    kefir_bool_t empty_scope = true;
    REQUIRE_OK(local_scope_empty(mem, root, &empty_scope));
    const kefir_size_t begin = kefir_ir_type_total_length(builder->type);
    if (!empty_scope) {
        *scope_type_layout = kefir_ast_new_type_layout(mem, NULL, 0, begin);
        REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_STRUCT, 0, 0));
    }
    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res;
    for (res = kefir_ast_identifier_flat_scope_iter(scope, &iter); res == KEFIR_OK;
         res = kefir_ast_identifier_flat_scope_next(scope, &iter)) {
        struct kefir_ir_typeentry *typeentry = NULL;
        if (!empty_scope) {
            typeentry = kefir_ir_type_at(builder->type, begin);
        }
        REQUIRE_OK(translate_local_scoped_identifier(mem, context, builder, iter.identifier, iter.value, env,
                                                     type_bundle, type_traits, module, local_layout, *scope_type_layout,
                                                     typeentry));
    }
    REQUIRE(res == KEFIR_ITERATOR_END, res);

    kefir_bool_t empty_children = true;
    for (struct kefir_tree_node *child = kefir_tree_first_child(root); child != NULL && empty_children;
         child = kefir_tree_next_sibling(child)) {
        REQUIRE_OK(local_scope_empty(mem, child, &empty_children));
    }
    if (!empty_children) {
        const kefir_size_t children_begin = kefir_ir_type_total_length(builder->type);
        struct kefir_ast_type_layout *sublocal_scopes_type_layout =
            kefir_ast_new_type_layout(mem, NULL, 0, children_begin);
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(builder->type, begin);
        typeentry->param++;
        REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_UNION, 0, 0));
        for (struct kefir_tree_node *child = kefir_tree_first_child(root); child != NULL;
             child = kefir_tree_next_sibling(child)) {
            struct kefir_ast_type_layout *sublocal_type_layout = NULL;
            struct kefir_ir_typeentry *child_typeentry = kefir_ir_type_at(builder->type, children_begin);
            child_typeentry->param++;
            REQUIRE_OK(traverse_local_scope(mem, context, child, builder, env, type_bundle, type_traits, module,
                                            local_layout, &sublocal_type_layout));
            if (sublocal_type_layout != NULL) {
                REQUIRE_OK(kefir_list_insert_after(
                    mem, &sublocal_scopes_type_layout->custom_layout.sublayouts,
                    kefir_list_tail(&sublocal_scopes_type_layout->custom_layout.sublayouts), sublocal_type_layout));
                sublocal_type_layout->parent = sublocal_scopes_type_layout;
            }
        }
        REQUIRE_OK(kefir_list_insert_after(mem, &(*scope_type_layout)->custom_layout.sublayouts,
                                           kefir_list_tail(&(*scope_type_layout)->custom_layout.sublayouts),
                                           sublocal_scopes_type_layout));
        sublocal_scopes_type_layout->parent = *scope_type_layout;
    } else {
        for (struct kefir_tree_node *child = kefir_tree_first_child(root); child != NULL;
             child = kefir_tree_next_sibling(child)) {
            struct kefir_ast_type_layout *sublocal_type_layout = NULL;
            REQUIRE_OK(traverse_local_scope(mem, context, child, builder, env, type_bundle, type_traits, module,
                                            local_layout, &sublocal_type_layout));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_build_local_scope_layout(struct kefir_mem *mem,
                                                             const struct kefir_ast_local_context *context,
                                                             const struct kefir_ast_translator_environment *env,
                                                             struct kefir_ir_module *module,
                                                             struct kefir_ast_translator_type_resolver *type_resolver,
                                                             struct kefir_ast_translator_local_scope_layout *layout) {
    UNUSED(type_resolver);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST local scope layout"));
    REQUIRE(layout->local_context == NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty AST translator local scope layout"));

    layout->local_context = context;

    if (!kefir_ast_identifier_block_scope_empty(&context->ordinary_scope)) {
        struct kefir_irbuilder_type builder;
        REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, layout->local_layout));
        REQUIRE_OK(traverse_local_scope(mem, &context->context, &context->ordinary_scope.root, &builder, env,
                                        context->context.type_bundle, context->context.type_traits, module, layout,
                                        &layout->local_type_layout));
        REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));

        if (layout->local_type_layout != NULL) {
            REQUIRE_OK(
                kefir_ast_translator_evaluate_type_layout(mem, env, layout->local_type_layout, layout->local_layout));
        }
    }

    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res;
    for (res = kefir_ast_identifier_flat_scope_iter(&context->label_scope, &iter); res == KEFIR_OK;
         res = kefir_ast_identifier_flat_scope_next(&context->label_scope, &iter)) {

        REQUIRE(iter.value->klass == KEFIR_AST_SCOPE_IDENTIFIER_LABEL,
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected label scope to contain only labels"));
        REQUIRE(iter.value->label.point->parent != NULL,
                KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Cannot translate undefined label"));
        REQUIRE_OK(kefir_ast_translator_flow_control_point_init(mem, iter.value->label.point, NULL));
    }
    return KEFIR_OK;
}
