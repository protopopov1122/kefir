#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast/type_conv.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t translate_parameters(struct kefir_mem *mem,
                                         struct kefir_ast_translator_context *context,
                                         struct kefir_irbuilder_block *builder,
                                         const struct kefir_ast_function_call *node,
                                         struct kefir_ast_translator_function_declaration *decl) {
    REQUIRE(decl->function_type->function_type.mode != KEFIR_AST_FUNCTION_TYPE_PARAM_IDENTIFIERS,
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Function declarations with identifier parameters are not implemented yet"));
    REQUIRE(!decl->ir_function_decl->vararg,
        KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Vararg function invocations are not implemented yet"));

    const struct kefir_list_entry *arg_value_iter = kefir_list_head(&node->arguments);
    const struct kefir_list_entry *decl_arg_iter = kefir_list_head(&decl->argument_layouts);
    for (; arg_value_iter != NULL && decl_arg_iter != NULL;
        kefir_list_next(&arg_value_iter), kefir_list_next(&decl_arg_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_type_layout *, parameter_layout,
            decl_arg_iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, parameter_value,
            arg_value_iter->value);

        REQUIRE_OK(kefir_ast_translate_expression(mem, parameter_value, builder, context));
        if (parameter_layout != NULL) {
            REQUIRE_OK(kefir_ast_translate_typeconv(builder, parameter_value->properties.type, parameter_layout->type));
        } else if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(parameter_value->properties.type)) {
            const struct kefir_ast_type *promoted_type =
                kefir_ast_type_int_promotion(context->ast_context->type_traits, parameter_value->properties.type);
            REQUIRE_OK(kefir_ast_translate_typeconv(builder, parameter_value->properties.type, promoted_type));
        } else if (parameter_value->properties.type->tag == KEFIR_AST_TYPE_SCALAR_FLOAT) {
            REQUIRE_OK(kefir_ast_translate_typeconv(builder, parameter_value->properties.type, kefir_ast_type_double()));
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
    REQUIRE(function_type->tag == KEFIR_AST_TYPE_FUNCTION, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected callable function"));

    const struct kefir_ast_translator_cached_type *cached_type = NULL;
    REQUIRE_OK(kefir_ast_translator_type_cache_generate_owned_function(mem, function_type,
        &context->type_cache, context->environment, context->module, &cached_type));
    REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_CACHED_FUNCTION_TYPE,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached function type"));

    REQUIRE_OK(translate_parameters(mem, context, builder, node, cached_type->function.declaration));
    if (cached_type->function.declaration->ir_function_decl->name == NULL) {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->function, builder, context));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_INVOKEV, cached_type->function.declaration->ir_function_decl->id));
    } else {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_INVOKE, cached_type->function.declaration->ir_function_decl->id));
    }
    return KEFIR_OK;
}