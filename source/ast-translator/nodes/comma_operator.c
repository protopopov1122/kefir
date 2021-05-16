#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_comma_operator_node(struct kefir_mem *mem,
                                                   struct kefir_ast_translator_context *context,
                                                   struct kefir_irbuilder_block *builder,
                                                   const struct kefir_ast_comma_operator *node) {
    UNUSED(mem);
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST comma operator node"));

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->expressions);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, expr,
            iter->value);
        REQUIRE_OK(kefir_ast_translate_expression(mem, expr, builder, context));
        if (iter->next != NULL && expr->properties.type->tag != KEFIR_AST_TYPE_VOID) {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    }
    return KEFIR_OK;
}
