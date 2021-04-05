#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_generic_selection_node(struct kefir_mem *mem,
                                                      const struct kefir_ast_translator_context *context,
                                                      struct kefir_irbuilder_block *builder,
                                                      const struct kefir_ast_generic_selection *node) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST generic selection node"));


    const struct kefir_ast_type *control_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle,
        node->control->properties.type);
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->associations);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc,
            iter->value);
        if (KEFIR_AST_TYPE_COMPATIBLE(context->ast_context->type_traits, control_type, assoc->type)) {
            return kefir_ast_translate_expression(mem, assoc->expr, builder, context);
        }
    }
    if (node->default_assoc != NULL) {
        return kefir_ast_translate_expression(mem, node->default_assoc, builder, context);
    }

    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected at least one of associations in generic selection to be compatible"
            " with control expression type");
}