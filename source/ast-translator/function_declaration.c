#include "kefir/ast-translator/function_declaration.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ir/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t kefir_ast_translator_function_declaration_alloc_args(struct kefir_mem *mem,
                                                                       const struct kefir_ast_translator_environment *env,
                                                                       const struct kefir_ast_type_traits *type_traits,
                                                                       const struct kefir_ast_type *func_type,
                                                                       const struct kefir_list *parameters,
                                                                       struct kefir_ast_translator_function_declaration *func_decl) {
    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, func_decl->ir_argument_type));

    const struct kefir_list_entry *param_iter = kefir_list_head(parameters);
    for (const struct kefir_list_entry *iter = kefir_list_head(&func_type->function_type.parameters);
        iter != NULL;
        kefir_list_next(&iter), kefir_list_next(&param_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter,
            iter->value);

        struct kefir_ast_type_layout *parameter_layout = NULL;
        if (parameter->adjusted_type != NULL) {
            if (parameter->adjusted_type->tag != KEFIR_AST_TYPE_VOID) {
                kefir_result_t res = kefir_ast_translate_object_type(mem, parameter->adjusted_type,
                    0, env, &builder, &parameter_layout);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    KEFIR_IRBUILDER_TYPE_FREE(&builder);
                    return res;
                });
            } else {
                continue;
            }
        }

        kefir_result_t res = kefir_list_insert_after(mem, &func_decl->argument_layouts,
            kefir_list_tail(&func_decl->argument_layouts), parameter_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, parameter_layout);
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });

        if (parameter->identifier != NULL) {
            res =  kefir_hashtree_insert(mem, &func_decl->named_argument_layouts,
                (kefir_hashtree_key_t) parameter->identifier, (kefir_hashtree_value_t) parameter_layout);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_IRBUILDER_TYPE_FREE(&builder);
                return res;
            });
        }
    }

    for (; param_iter != NULL; kefir_list_next(&param_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param,
            param_iter->value);
        
        const struct kefir_ast_type *param_type = kefir_ast_translator_normalize_type(param->properties.type);
        if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(param_type)) {
            param_type = kefir_ast_type_int_promotion(type_traits, param_type);
        } else if (param_type->tag == KEFIR_AST_TYPE_SCALAR_FLOAT) {
            param_type = kefir_ast_type_double();
        }

        struct kefir_ast_type_layout *parameter_layout = NULL;
        kefir_result_t res = kefir_ast_translate_object_type(mem, param_type,
            0, env, &builder, &parameter_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });

        res = kefir_list_insert_after(mem, &func_decl->argument_layouts,
            kefir_list_tail(&func_decl->argument_layouts), parameter_layout);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_type_layout_free(mem, parameter_layout);
            KEFIR_IRBUILDER_TYPE_FREE(&builder);
            return res;
        });
    }
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    return KEFIR_OK;
}


static kefir_result_t kefir_ast_translator_function_declaration_alloc_return(struct kefir_mem *mem,
                                                                         const struct kefir_ast_translator_environment *env,
                                                                         const struct kefir_ast_type *func_type,
                                                                         struct kefir_ast_translator_function_declaration *func_decl) {
    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, func_decl->ir_return_type));

    kefir_result_t res = kefir_ast_translate_object_type(mem, func_type->function_type.return_type, 0,
        env, &builder, &func_decl->return_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&builder);
        return res;
    });

    res = KEFIR_IRBUILDER_TYPE_FREE(&builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_layout_free(mem, func_decl->return_layout);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t free_argument_layout(struct kefir_mem *mem,
                                         struct kefir_list *list,
                                         struct kefir_list_entry *entry,
                                         void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, type_layout,
        entry->value);
    REQUIRE_OK(kefir_ast_type_layout_free(mem, type_layout));
    return KEFIR_OK;
}

static kefir_result_t kefir_ast_translator_function_declaration_alloc(struct kefir_mem *mem,
                                                                  const struct kefir_ast_translator_environment *env,
                                                                  const struct kefir_ast_type_traits *type_traits,
                                                                  struct kefir_ir_module *module,
                                                                  const struct kefir_ast_type *func_type,
                                                                  const struct kefir_list *parameters,
                                                                  struct kefir_ast_translator_function_declaration *func_decl) {
    func_decl->function_type = func_type;
    REQUIRE_OK(kefir_list_init(&func_decl->argument_layouts));
    REQUIRE_OK(kefir_hashtree_init(&func_decl->named_argument_layouts, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_list_on_remove(&func_decl->argument_layouts, free_argument_layout, NULL));
    func_decl->ir_argument_type = kefir_ir_module_new_type(mem, module, 0, &func_decl->ir_argument_type_id);
    REQUIRE(func_decl->ir_argument_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR type"));
    func_decl->ir_return_type = kefir_ir_module_new_type(mem, module, 0, &func_decl->ir_return_type_id);
    REQUIRE(func_decl->ir_return_type != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR type"));
    kefir_result_t res = kefir_ast_translator_function_declaration_alloc_args(mem, env, type_traits, func_type, parameters, func_decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &func_decl->named_argument_layouts);
        kefir_list_free(mem, &func_decl->argument_layouts);
        return res;
    });

    res = kefir_ast_translator_function_declaration_alloc_return(mem, env, func_type, func_decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &func_decl->named_argument_layouts);
        kefir_list_free(mem, &func_decl->argument_layouts);
        return res;
    });

    func_decl->ir_function_decl = kefir_ir_module_new_function_declaration(mem, module, func_type->function_type.identifier,
        func_decl->ir_argument_type, func_type->function_type.ellipsis, func_decl->ir_return_type);
    REQUIRE_ELSE(func_decl->ir_function_decl != NULL, {
        kefir_ast_type_layout_free(mem, func_decl->return_layout);
        kefir_hashtree_free(mem, &func_decl->named_argument_layouts);
        kefir_list_free(mem, &func_decl->argument_layouts);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR function declaration");
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_declaration_init(struct kefir_mem *mem,
                                                          const struct kefir_ast_translator_environment *env,
                                                          const struct kefir_ast_type_traits *type_traits,
                                                          struct kefir_ir_module *module,
                                                          const struct kefir_ast_type *func_type,
                                                          struct kefir_ast_translator_function_declaration **func_decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(func_type != NULL && func_type->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(func_decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator function declaration pointer"));

    struct kefir_ast_translator_function_declaration *function_declaration = 
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_function_declaration));
    REQUIRE(function_declaration != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator function declaration"));

    kefir_result_t res = kefir_ast_translator_function_declaration_alloc(mem, env, type_traits, module, func_type, NULL, function_declaration);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, function_declaration);
        return res;
    });

    *func_decl = function_declaration;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_declaration_free(struct kefir_mem *mem,
                                                          struct kefir_ast_translator_function_declaration *func_decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(func_decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator function declaration pointer"));

    REQUIRE_OK(kefir_ast_type_layout_free(mem, func_decl->return_layout));
    REQUIRE_OK(kefir_hashtree_free(mem, &func_decl->named_argument_layouts));
    REQUIRE_OK(kefir_list_free(mem, &func_decl->argument_layouts));
    func_decl->ir_argument_type = NULL;
    func_decl->ir_return_type = NULL;
    func_decl->function_type = NULL;
    KEFIR_FREE(mem, func_decl);
    return KEFIR_OK;   
}

kefir_result_t kefir_ast_translator_function_declaration_init_vararg(struct kefir_mem *mem,
                                                                 const struct kefir_ast_translator_environment *env,
                                                                 const struct kefir_ast_type_traits *type_traits,
                                                                 struct kefir_ir_module *module,
                                                                 const struct kefir_ast_type *func_type,
                                                                 const struct kefir_list *parameters,
                                                                 struct kefir_ast_translator_function_declaration **func_decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator environment"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(func_type != NULL && func_type->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(parameters != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function call parameters"));
    REQUIRE(func_decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator function declaration pointer"));

    struct kefir_ast_translator_function_declaration *function_declaration = 
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_function_declaration));
    REQUIRE(function_declaration != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator function declaration"));

    kefir_result_t res = kefir_ast_translator_function_declaration_alloc(mem, env, type_traits, module, func_type, parameters, function_declaration);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, function_declaration);
        return res;
    });

    *func_decl = function_declaration;
    return KEFIR_OK;
}