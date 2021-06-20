#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_conditional_statement_node(struct kefir_mem *mem,
                                                              struct kefir_ast_translator_context *context,
                                                              struct kefir_irbuilder_block *builder,
                                                              const struct kefir_ast_conditional_statement *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST conditional statement node"));

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->condition, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
    REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(
        mem, node->base.properties.statement_props.flow_control_statement->value.conditional.thenBranchEnd,
        builder->block, KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));

    REQUIRE_OK(kefir_ast_translate_statement(mem, node->thenBranch, builder, context));
    if (node->elseBranch != NULL) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_JMP, 0));
        REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(
            mem, node->base.properties.statement_props.flow_control_statement->value.conditional.elseBranchEnd,
            builder->block, KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));
    }
    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(
        mem, node->base.properties.statement_props.flow_control_statement->value.conditional.thenBranchEnd,
        KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));

    if (node->elseBranch != NULL) {
        REQUIRE_OK(kefir_ast_translate_statement(mem, node->elseBranch, builder, context));
        REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(
            mem, node->base.properties.statement_props.flow_control_statement->value.conditional.elseBranchEnd,
            KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));
    }

    return KEFIR_OK;
}
