#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_array_subscript_node(struct kefir_mem *mem,
                                                    struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_array_subscript *node) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST array subscript node"));

    const struct kefir_ast_translator_cached_type *cached_type = NULL;
    REQUIRE_OK(kefir_ast_translator_type_cache_generate_owned(mem, node->base.properties.type, 0,
        &context->type_cache, context->environment, context->module, &cached_type));

    const struct kefir_ast_type *array_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem,
        context->ast_context->type_bundle, node->array->properties.type);
    if (array_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->array, builder, context));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->subscript, builder, context));
    } else {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->subscript, builder, context));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->array, builder, context));
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->ir_type_id, 0));
    REQUIRE_OK(kefir_ast_translator_resolve_value(node->base.properties.type, builder));
    return KEFIR_OK;
}