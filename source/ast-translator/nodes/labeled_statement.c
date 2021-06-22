#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_labeled_statement_node(struct kefir_mem *mem,
                                                          struct kefir_ast_translator_context *context,
                                                          struct kefir_irbuilder_block *builder,
                                                          const struct kefir_ast_labeled_statement *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST labeled statement node"));

    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(
        mem, node->base.properties.statement_props.flow_control_point, KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));
    REQUIRE_OK(kefir_ast_translate_statement(mem, node->statement, builder, context));
    return KEFIR_OK;
}
