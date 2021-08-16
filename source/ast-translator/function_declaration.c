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

#include "kefir/ast-translator/function_declaration.h"
#include "kefir/ast-translator/type_resolver.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/layout.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static const struct kefir_ast_type *adjust_untyped_parameter(struct kefir_mem *mem,
                                                             struct kefir_ast_type_bundle *type_bundle,
                                                             const struct kefir_ast_type_traits *type_traits,
                                                             const struct kefir_ast_type *original) {
    const struct kefir_ast_type *param_type =
        kefir_ast_translator_normalize_type(KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, type_bundle, original));
    if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(param_type)) {
        return kefir_ast_type_int_promotion(type_traits, param_type);
    } else if (param_type->tag == KEFIR_AST_TYPE_SCALAR_FLOAT) {
        return kefir_ast_type_double();
    } else {
        return param_type;
    }
}

static kefir_result_t kefir_ast_translator_function_declaration_alloc_args(
    struct kefir_mem *mem, const struct kefir_ast_translator_environment *env,
    struct kefir_ast_type_bundle *type_bundle, const struct kefir_ast_type_traits *type_traits,
    struct kefir_ast_translator_type_resolver *type_resolver, const struct kefir_ast_type *func_type,
    const struct kefir_list *parameters, struct kefir_ast_translator_function_declaration *func_decl) {
    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, func_decl->ir_argument_type));

    const struct kefir_list_entry *param_iter = kefir_list_head(parameters);
    for (const struct kefir_list_entry *iter = kefir_list_head(&func_type->function_type.parameters); iter != NULL;
         kefir_list_next(&iter), kefir_list_next(&param_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter, iter->value);

        struct kefir_ast_type_layout *parameter_layout = NULL;
        const struct kefir_ast_type *param_type = NULL;
        if (parameter->adjusted_type != NULL) {
            param_type = parameter->adjusted_type;
        } else if (param_iter != NULL) {
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, param_iter->value);
            REQUIRE(param->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION ||
                        param->properties.category == KEFIR_AST_NODE_CATEGORY_INIT_DECLARATOR,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                    "Function declaration parameter shall be either expression, or declaration"));
            param_type = adjust_untyped_parameter(mem, type_bundle, type_traits, param->properties.type);
            REQUIRE(param_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to adjust untyped parameter"));
        }

        if (param_type != NULL) {
            kefir_result_t res = kefir_ast_translate_object_type(mem, param_type, 0, env, &builder, &parameter_layout);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_IRBUILDER_TYPE_FREE(&builder);
                return res;
            });

            res = kefir_ast_translator_evaluate_type_layout(mem, env, parameter_layout, func_decl->ir_argument_type);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_type_layout_free(mem, parameter_layout);
                KEFIR_IRBUILDER_TYPE_FREE(&builder);
                return res;
            });
        }

        kefir_result_t res = kefir_list_insert_after(mem, &func_decl->argument_layouts,
                                                     kefir_list_tail(&func_decl->argument_layouts), parameter_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, parameter_layout);
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });

        if (parameter->identifier != NULL) {
            res = kefir_hashtree_insert(mem, &func_decl->named_argument_layouts,
                                        (kefir_hashtree_key_t) parameter->identifier,
                                        (kefir_hashtree_value_t) parameter_layout);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_IRBUILDER_TYPE_FREE(&builder);
                return res;
            });
        }

        if (parameter_layout != NULL) {
            res = KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_REGISTER_OBJECT(mem, type_resolver, func_decl->ir_argument_type_id,
                                                                     func_decl->ir_argument_type, parameter_layout);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_IRBUILDER_TYPE_FREE(&builder);
                return res;
            });
        }
    }

    for (; param_iter != NULL; kefir_list_next(&param_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param, param_iter->value);

        const struct kefir_ast_type *param_type =
            adjust_untyped_parameter(mem, type_bundle, type_traits, param->properties.type);
        REQUIRE(param_type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to adjust untyped parameter"));

        struct kefir_ast_type_layout *parameter_layout = NULL;
        kefir_result_t res = kefir_ast_translate_object_type(mem, param_type, 0, env, &builder, &parameter_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });

        res = kefir_ast_translator_evaluate_type_layout(mem, env, parameter_layout, func_decl->ir_argument_type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, parameter_layout);
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });

        res = kefir_list_insert_after(mem, &func_decl->argument_layouts, kefir_list_tail(&func_decl->argument_layouts),
                                      parameter_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, parameter_layout);
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });

        res = KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_REGISTER_OBJECT(mem, type_resolver, func_decl->ir_argument_type_id,
                                                                 func_decl->ir_argument_type, parameter_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });
    }
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    return KEFIR_OK;
}

static kefir_result_t kefir_ast_translator_function_declaration_alloc_return(
    struct kefir_mem *mem, const struct kefir_ast_translator_environment *env,
    struct kefir_ast_translator_type_resolver *type_resolver, const struct kefir_ast_type *func_type,
    struct kefir_ast_translator_function_declaration *func_decl) {
    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, func_decl->ir_return_type));
    kefir_result_t res = kefir_ast_translate_object_type(mem, func_type->function_type.return_type, 0, env, &builder,
                                                         &func_decl->return_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&builder);
        return res;
    });
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    res = kefir_ast_translator_evaluate_type_layout(mem, env, func_decl->return_layout, func_decl->ir_return_type);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_layout_free(mem, func_decl->return_layout);
        return res;
    });
    if (func_type->function_type.return_type->tag != KEFIR_AST_TYPE_VOID) {
        REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_REGISTER_OBJECT(
            mem, type_resolver, func_decl->ir_return_type_id, func_decl->ir_return_type, func_decl->return_layout));
    }
    return KEFIR_OK;
}

static kefir_result_t free_argument_layout(struct kefir_mem *mem, struct kefir_list *list,
                                           struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, type_layout, entry->value);
    if (type_layout != NULL) {
        REQUIRE_OK(kefir_ast_type_layout_free(mem, type_layout));
    }
    return KEFIR_OK;
}

static kefir_result_t kefir_ast_translator_function_declaration_alloc(
    struct kefir_mem *mem, const struct kefir_ast_translator_environment *env,
    struct kefir_ast_type_bundle *type_bundle, const struct kefir_ast_type_traits *type_traits,
    struct kefir_ir_module *module, struct kefir_ast_translator_type_resolver *type_resolver,
    const struct kefir_ast_type *func_type, const struct kefir_list *parameters,
    struct kefir_ast_translator_function_declaration *func_decl) {
    func_decl->function_type = func_type;
    REQUIRE_OK(kefir_list_init(&func_decl->argument_layouts));
    REQUIRE_OK(kefir_hashtree_init(&func_decl->named_argument_layouts, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_list_on_remove(&func_decl->argument_layouts, free_argument_layout, NULL));
    func_decl->ir_argument_type = kefir_ir_module_new_type(mem, module, 0, &func_decl->ir_argument_type_id);
    REQUIRE(func_decl->ir_argument_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR type"));
    func_decl->ir_return_type = kefir_ir_module_new_type(mem, module, 0, &func_decl->ir_return_type_id);
    REQUIRE(func_decl->ir_return_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR type"));
    kefir_result_t res = kefir_ast_translator_function_declaration_alloc_args(
        mem, env, type_bundle, type_traits, type_resolver, func_type, parameters, func_decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &func_decl->named_argument_layouts);
        kefir_list_free(mem, &func_decl->argument_layouts);
        return res;
    });

    res = kefir_ast_translator_function_declaration_alloc_return(mem, env, type_resolver, func_type, func_decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &func_decl->named_argument_layouts);
        kefir_list_free(mem, &func_decl->argument_layouts);
        return res;
    });

    func_decl->ir_function_decl = kefir_ir_module_new_function_declaration(
        mem, module, func_type->function_type.identifier, func_decl->ir_argument_type,
        func_type->function_type.ellipsis, func_decl->ir_return_type);
    REQUIRE_ELSE(func_decl->ir_function_decl != NULL, {
        kefir_hashtree_free(mem, &func_decl->named_argument_layouts);
        kefir_list_free(mem, &func_decl->argument_layouts);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR function declaration");
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_declaration_init(
    struct kefir_mem *mem, const struct kefir_ast_translator_environment *env,
    struct kefir_ast_type_bundle *type_bundle, const struct kefir_ast_type_traits *type_traits,
    struct kefir_ir_module *module, struct kefir_ast_translator_type_resolver *type_resolver,
    const struct kefir_ast_type *func_type, const struct kefir_list *parameters,
    struct kefir_ast_translator_function_declaration **func_decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(type_resolver != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator type resolver"));
    REQUIRE(func_type != NULL && func_type->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(func_decl != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator function declaration pointer"));

    struct kefir_ast_translator_function_declaration *function_declaration =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_function_declaration));
    REQUIRE(function_declaration != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator function declaration"));

    kefir_result_t res = kefir_ast_translator_function_declaration_alloc(
        mem, env, type_bundle, type_traits, module, type_resolver, func_type, parameters, function_declaration);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, function_declaration);
        return res;
    });

    *func_decl = function_declaration;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_declaration_free(
    struct kefir_mem *mem, struct kefir_ast_translator_function_declaration *func_decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(func_decl != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator function declaration pointer"));

    REQUIRE_OK(kefir_ast_type_layout_free(mem, func_decl->return_layout));
    REQUIRE_OK(kefir_hashtree_free(mem, &func_decl->named_argument_layouts));
    REQUIRE_OK(kefir_list_free(mem, &func_decl->argument_layouts));
    func_decl->ir_argument_type = NULL;
    func_decl->ir_return_type = NULL;
    func_decl->function_type = NULL;
    KEFIR_FREE(mem, func_decl);
    return KEFIR_OK;
}
