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

#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast/runtime.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/type_tree.h"

static kefir_size_t resolve_base_slot(const struct kefir_ir_type_tree_node *node) {
    REQUIRE(node != NULL, 0);
    return resolve_base_slot(node->parent) + node->relative_slot;
}

static kefir_result_t initialize_data(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                      struct kefir_ir_module *module, const struct kefir_ir_type *type,
                                      struct kefir_ast_type_layout *type_layout,
                                      struct kefir_ast_initializer *initializer, struct kefir_ir_data *data) {

    struct kefir_ir_type_tree type_tree;
    REQUIRE_OK(kefir_ir_type_tree_init(mem, type, &type_tree));

    const struct kefir_ir_type_tree_node *tree_node;
    kefir_result_t res = kefir_ir_type_tree_at(&type_tree, type_layout->value, &tree_node);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_tree_free(mem, &type_tree);
        return res;
    });

    res = kefir_ast_translate_data_initializer(mem, context, module, type_layout, type, initializer, data,
                                               resolve_base_slot(tree_node));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_tree_free(mem, &type_tree);
        return res;
    });

    REQUIRE_OK(kefir_ir_type_tree_free(mem, &type_tree));
    return KEFIR_OK;
}

static kefir_result_t translate_externals(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                          struct kefir_ir_module *module,
                                          const struct kefir_ast_translator_global_scope_layout *global_scope) {
    for (const struct kefir_list_entry *iter = kefir_list_head(&global_scope->external_objects); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier, iter->value);
        switch (scoped_identifier->value->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                                 scoped_identifier->value->payload.ptr);
                if (scoped_identifier->value->object.external) {
                    REQUIRE_OK(kefir_ir_module_declare_external(mem, module, scoped_identifier->identifier));
                } else {
                    struct kefir_ir_data *data =
                        kefir_ir_module_new_named_data(mem, module, scoped_identifier->identifier,
                                                       KEFIR_IR_DATA_GLOBAL_STORAGE, identifier_data->type_id);
                    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate IR named data"));
                    if (scoped_identifier->value->object.initializer != NULL) {
                        REQUIRE_OK(initialize_data(mem, context, module, identifier_data->type, identifier_data->layout,
                                                   scoped_identifier->value->object.initializer, data));
                    }
                    REQUIRE_OK(kefir_ir_data_finalize(data));

                    REQUIRE_OK(kefir_ir_module_declare_global(mem, module, scoped_identifier->identifier));
                }
            } break;

            case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION: {
                if (scoped_identifier->value->function.external) {
                    REQUIRE_OK(kefir_ir_module_declare_external(mem, module, scoped_identifier->identifier));
                } else if (scoped_identifier->value->function.storage != KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC) {
                    REQUIRE_OK(kefir_ir_module_declare_global(mem, module, scoped_identifier->identifier));
                }
            } break;

            default:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unable to translate global scoped identifier");
        }
    }
    return KEFIR_OK;
}

static kefir_result_t get_static_data(struct kefir_mem *mem, struct kefir_ir_module *module, kefir_id_t type_id,
                                      struct kefir_ir_data **data) {
    *data = kefir_ir_module_get_named_data(module, KEFIR_AST_TRANSLATOR_STATIC_VARIABLES_IDENTIFIER);
    if (*data == NULL) {
        *data = kefir_ir_module_new_named_data(mem, module, KEFIR_AST_TRANSLATOR_STATIC_VARIABLES_IDENTIFIER,
                                               KEFIR_IR_DATA_GLOBAL_STORAGE, type_id);
        REQUIRE(*data != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR data"));
    } else {
        REQUIRE((*data)->type_id == type_id,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Provided IR type id does not match existing"));
    }
    return KEFIR_OK;
}

static kefir_result_t get_static_uninit_data(struct kefir_mem *mem, struct kefir_ir_module *module, kefir_id_t type_id,
                                             struct kefir_ir_data **data) {
    *data = kefir_ir_module_get_named_data(module, KEFIR_AST_TRANSLATOR_STATIC_UNINIT_VARIABLES_IDENTIFIER);
    if (*data == NULL) {
        *data = kefir_ir_module_new_named_data(mem, module, KEFIR_AST_TRANSLATOR_STATIC_UNINIT_VARIABLES_IDENTIFIER,
                                               KEFIR_IR_DATA_GLOBAL_STORAGE, type_id);
        REQUIRE(*data != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR data"));
    } else {
        REQUIRE((*data)->type_id == type_id,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Provided IR type id does not match existing"));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_static(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                       struct kefir_ir_module *module,
                                       const struct kefir_ast_translator_global_scope_layout *global_scope) {
    struct kefir_ir_data *static_data = NULL;
    if (kefir_ir_type_total_length(global_scope->static_layout) > 0) {
        REQUIRE_OK(get_static_data(mem, module, global_scope->static_layout_id, &static_data));
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&global_scope->static_objects); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier, iter->value);

        switch (scoped_identifier->value->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                                 scoped_identifier->value->payload.ptr);
                if (scoped_identifier->value->object.initializer != NULL) {
                    REQUIRE_OK(initialize_data(mem, context, module, identifier_data->type, identifier_data->layout,
                                               scoped_identifier->value->object.initializer, static_data));
                }
            } break;

            case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
                // Do nothing
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unable to translate global scoped identifier");
        }
    }

    if (static_data != NULL) {
        REQUIRE_OK(kefir_ir_data_finalize(static_data));
    }

    if (kefir_ir_type_total_length(global_scope->static_uninit_layout) > 0) {
        struct kefir_ir_data *static_uninit_data;
        REQUIRE_OK(get_static_uninit_data(mem, module, global_scope->static_uninit_layout_id, &static_uninit_data));
        REQUIRE_OK(kefir_ir_data_finalize(static_uninit_data));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_external_thread_locals(
    struct kefir_mem *mem, const struct kefir_ast_context *context, struct kefir_ir_module *module,
    const struct kefir_ast_translator_global_scope_layout *global_scope) {
    for (const struct kefir_list_entry *iter = kefir_list_head(&global_scope->external_thread_local_objects);
         iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier, iter->value);
        switch (scoped_identifier->value->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                                 scoped_identifier->value->payload.ptr);
                if (scoped_identifier->value->object.external) {
                    REQUIRE_OK(kefir_ir_module_declare_external(mem, module, scoped_identifier->identifier));
                } else {
                    struct kefir_ir_data *data =
                        kefir_ir_module_new_named_data(mem, module, scoped_identifier->identifier,
                                                       KEFIR_IR_DATA_THREAD_LOCAL_STORAGE, identifier_data->type_id);
                    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate IR named data"));
                    if (scoped_identifier->value->object.initializer != NULL) {
                        REQUIRE_OK(initialize_data(mem, context, module, identifier_data->type, identifier_data->layout,
                                                   scoped_identifier->value->object.initializer, data));
                    }
                    REQUIRE_OK(kefir_ir_data_finalize(data));

                    REQUIRE_OK(kefir_ir_module_declare_global(mem, module, scoped_identifier->identifier));
                }
            } break;

            default:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unable to translate global scoped identifier");
        }
    }
    return KEFIR_OK;
}

static kefir_result_t get_static_thread_local_data(struct kefir_mem *mem, struct kefir_ir_module *module,
                                                   kefir_id_t type_id, struct kefir_ir_data **data) {
    *data = kefir_ir_module_get_named_data(module, KEFIR_AST_TRANSLATOR_STATIC_THREAD_LOCAL_VARIABLES_IDENTIFIER);
    if (*data == NULL) {
        *data =
            kefir_ir_module_new_named_data(mem, module, KEFIR_AST_TRANSLATOR_STATIC_THREAD_LOCAL_VARIABLES_IDENTIFIER,
                                           KEFIR_IR_DATA_THREAD_LOCAL_STORAGE, type_id);
        REQUIRE(*data != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR data"));
    } else {
        REQUIRE((*data)->type_id == type_id,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Provided IR type id does not match existing"));
    }
    return KEFIR_OK;
}

static kefir_result_t get_static_thread_local_uninit_data(struct kefir_mem *mem, struct kefir_ir_module *module,
                                                          kefir_id_t type_id, struct kefir_ir_data **data) {
    *data =
        kefir_ir_module_get_named_data(module, KEFIR_AST_TRANSLATOR_STATIC_THREAD_LOCAL_UNINIT_VARIABLES_IDENTIFIER);
    if (*data == NULL) {
        *data = kefir_ir_module_new_named_data(mem, module,
                                               KEFIR_AST_TRANSLATOR_STATIC_THREAD_LOCAL_UNINIT_VARIABLES_IDENTIFIER,
                                               KEFIR_IR_DATA_THREAD_LOCAL_STORAGE, type_id);
        REQUIRE(*data != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR data"));
    } else {
        REQUIRE((*data)->type_id == type_id,
                KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Provided IR type id does not match existing"));
    }
    return KEFIR_OK;
}

static kefir_result_t translate_static_thread_locals(
    struct kefir_mem *mem, const struct kefir_ast_context *context, struct kefir_ir_module *module,
    const struct kefir_ast_translator_global_scope_layout *global_scope) {
    struct kefir_ir_data *static_tlocal_data = NULL;
    if (kefir_ir_type_total_length(global_scope->static_thread_local_layout) > 0) {
        REQUIRE_OK(get_static_thread_local_data(mem, module, global_scope->static_thread_local_layout_id,
                                                &static_tlocal_data));
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&global_scope->static_thread_local_objects);
         iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier, iter->value);

        switch (scoped_identifier->value->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                                 scoped_identifier->value->payload.ptr);
                if (scoped_identifier->value->object.initializer != NULL) {
                    REQUIRE_OK(initialize_data(mem, context, module, identifier_data->type, identifier_data->layout,
                                               scoped_identifier->value->object.initializer, static_tlocal_data));
                }
            } break;

            default:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unable to translate global scoped identifier");
        }
    }

    if (static_tlocal_data != NULL) {
        REQUIRE_OK(kefir_ir_data_finalize(static_tlocal_data));
    }

    if (kefir_ir_type_total_length(global_scope->static_thread_local_uninit_layout) > 0) {
        struct kefir_ir_data *static_thread_local_uninit_data;
        REQUIRE_OK(get_static_thread_local_uninit_data(mem, module, global_scope->static_thread_local_uninit_layout_id,
                                                       &static_thread_local_uninit_data));
        REQUIRE_OK(kefir_ir_data_finalize(static_thread_local_uninit_data));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_global_scope(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                struct kefir_ir_module *module,
                                                const struct kefir_ast_translator_global_scope_layout *global_scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(global_scope != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator global scope"));

    REQUIRE_OK(translate_externals(mem, context, module, global_scope));
    REQUIRE_OK(translate_static(mem, context, module, global_scope));
    REQUIRE_OK(translate_external_thread_locals(mem, context, module, global_scope));
    REQUIRE_OK(translate_static_thread_locals(mem, context, module, global_scope));
    return KEFIR_OK;
}

static kefir_result_t translate_local_static(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                             struct kefir_ir_module *module,
                                             const struct kefir_ast_translator_local_scope_layout *local_scope) {
    struct kefir_ir_data *static_data = NULL;
    if (kefir_ir_type_total_length(local_scope->global->static_layout) > 0) {
        REQUIRE_OK(get_static_data(mem, module, local_scope->global->static_layout_id, &static_data));
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&local_scope->static_objects); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier, iter->value);

        switch (scoped_identifier->value->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                                 scoped_identifier->value->payload.ptr);
                if (scoped_identifier->value->object.initializer != NULL) {
                    REQUIRE_OK(initialize_data(mem, context, module, identifier_data->type, identifier_data->layout,
                                               scoped_identifier->value->object.initializer, static_data));
                }
            } break;

            case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
                // Do nothing
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unable to translate local scope identifier");
        }
    }
    return KEFIR_OK;
}

static kefir_result_t translate_local_static_thread_locals(
    struct kefir_mem *mem, const struct kefir_ast_context *context, struct kefir_ir_module *module,
    const struct kefir_ast_translator_local_scope_layout *local_scope) {
    struct kefir_ir_data *static_tlocal_data = NULL;
    if (kefir_ir_type_total_length(local_scope->global->static_thread_local_layout) > 0) {
        REQUIRE_OK(get_static_thread_local_data(mem, module, local_scope->global->static_thread_local_layout_id,
                                                &static_tlocal_data));
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&local_scope->static_thread_local_objects); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier, iter->value);

        switch (scoped_identifier->value->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                                 scoped_identifier->value->payload.ptr);
                if (scoped_identifier->value->object.initializer != NULL) {
                    REQUIRE_OK(initialize_data(mem, context, module, identifier_data->type, identifier_data->layout,
                                               scoped_identifier->value->object.initializer, static_tlocal_data));
                }
            } break;

            default:
                return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unable to translate local scope identifier");
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_local_scope(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                               struct kefir_ir_module *module,
                                               const struct kefir_ast_translator_local_scope_layout *local_scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(local_scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST translator local scope"));

    REQUIRE_OK(translate_local_static(mem, context, module, local_scope));
    REQUIRE_OK(translate_local_static_thread_locals(mem, context, module, local_scope));
    return KEFIR_OK;
}
