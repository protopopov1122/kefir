#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t resolve_cached_type(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                          const struct kefir_ast_type *function_type,
                                          const struct kefir_ast_translator_resolved_type **cached_type) {
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(
        mem, &context->type_cache.resolver, context->environment, context->ast_context->type_bundle,
        context->ast_context->type_traits, context->module, function_type, cached_type));
    REQUIRE((*cached_type)->klass == KEFIR_AST_TRANSLATOR_RESOLVED_FUNCTION_TYPE,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached function type"));
    return KEFIR_OK;
}

static kefir_result_t translate_parameters(struct kefir_mem *mem, struct kefir_ast_translator_context *context,
                                           struct kefir_irbuilder_block *builder,
                                           const struct kefir_ast_function_call *node,
                                           struct kefir_ast_translator_function_declaration *func_decl) {
    const struct kefir_list_entry *arg_value_iter = kefir_list_head(&node->arguments);
    const struct kefir_list_entry *decl_arg_iter = kefir_list_head(&func_decl->argument_layouts);
    for (; arg_value_iter != NULL && decl_arg_iter != NULL;
         kefir_list_next(&arg_value_iter), kefir_list_next(&decl_arg_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, parameter_layout, decl_arg_iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, parameter_value, arg_value_iter->value);

        REQUIRE_OK(kefir_ast_translate_expression(mem, parameter_value, builder, context));
        if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(parameter_value->properties.type)) {
            REQUIRE_OK(kefir_ast_translate_typeconv(builder, context->ast_context->type_traits,
                                                    parameter_value->properties.type, parameter_layout->type));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_function_call_node(struct kefir_mem *mem,
                                                      struct kefir_ast_translator_context *context,
                                                      struct kefir_irbuilder_block *builder,
                                                      const struct kefir_ast_function_call *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function call node"));

    const struct kefir_ast_type *function_type = kefir_ast_translator_normalize_type(node->function->properties.type);
    if (function_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        function_type = function_type->referenced_type;
    }
    REQUIRE(function_type->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected callable function"));

    struct kefir_ir_function_decl *ir_decl = NULL;
    switch (function_type->function_type.mode) {
        case KEFIR_AST_FUNCTION_TYPE_PARAMETERS:
            if (function_type->function_type.ellipsis) {
                struct kefir_ast_translator_function_declaration *func_decl = NULL;
                REQUIRE_OK(kefir_ast_translator_function_declaration_init(
                    mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                    context->module, &context->type_cache.resolver, function_type, &node->arguments, &func_decl));
                ir_decl = func_decl->ir_function_decl;
                if (ir_decl->name == NULL) {
                    REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
                }
                kefir_result_t res = translate_parameters(mem, context, builder, node, func_decl);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_ast_translator_function_declaration_free(mem, func_decl);
                    return res;
                });
                REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, func_decl));
            } else {
                const struct kefir_ast_translator_resolved_type *cached_type = NULL;
                REQUIRE_OK(resolve_cached_type(mem, context, function_type, &cached_type));
                ir_decl = cached_type->function.declaration->ir_function_decl;
                if (ir_decl->name == NULL) {
                    REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
                }
                REQUIRE_OK(translate_parameters(mem, context, builder, node, cached_type->function.declaration));
            }
            break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS: {
            struct kefir_ast_translator_function_declaration *func_decl = NULL;
            REQUIRE_OK(kefir_ast_translator_function_declaration_init(
                mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                context->module, &context->type_cache.resolver, function_type, &node->arguments, &func_decl));
            ir_decl = func_decl->ir_function_decl;
            if (ir_decl->name == NULL) {
                REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
            }
            kefir_result_t res = translate_parameters(mem, context, builder, node, func_decl);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_translator_function_declaration_free(mem, func_decl);
                return res;
            });
            REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, func_decl));
        } break;

        case KEFIR_AST_FUNCTION_TYPE_PARAM_EMPTY: {
            if (kefir_list_length(&node->arguments) == 0) {
                const struct kefir_ast_translator_resolved_type *cached_type = NULL;
                REQUIRE_OK(resolve_cached_type(mem, context, function_type, &cached_type));
                ir_decl = cached_type->function.declaration->ir_function_decl;
                if (ir_decl->name == NULL) {
                    REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
                }
            } else {
                struct kefir_ast_translator_function_declaration *func_decl = NULL;
                REQUIRE_OK(kefir_ast_translator_function_declaration_init(
                    mem, context->environment, context->ast_context->type_bundle, context->ast_context->type_traits,
                    context->module, &context->type_cache.resolver, function_type, &node->arguments, &func_decl));
                ir_decl = func_decl->ir_function_decl;
                if (ir_decl->name == NULL) {
                    REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
                }
                kefir_result_t res = translate_parameters(mem, context, builder, node, func_decl);
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_ast_translator_function_declaration_free(mem, func_decl);
                    return res;
                });
                REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, func_decl));
            }
        } break;
    }

    if (ir_decl->name == NULL) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_INVOKEV, ir_decl->id));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_INVOKE, ir_decl->id));
    }
    return KEFIR_OK;
}
