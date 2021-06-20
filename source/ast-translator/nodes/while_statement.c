#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_while_statement_node(struct kefir_mem *mem,
                                                        struct kefir_ast_translator_context *context,
                                                        struct kefir_irbuilder_block *builder,
                                                        const struct kefir_ast_while_statement *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST while statement node"));

    struct kefir_ast_flow_control_statement *flow_control_stmt =
        node->base.properties.statement_props.flow_control_statement;
    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(mem, flow_control_stmt->value.loop.continuation,
                                                               KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->controlling_expr, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
    REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(mem, flow_control_stmt->value.loop.end, builder->block,
                                                                 KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));

    REQUIRE_OK(kefir_ast_translate_statement(mem, node->body, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_JMP, 0));
    REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(mem, flow_control_stmt->value.loop.continuation,
                                                                 builder->block,
                                                                 KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));

    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(mem, flow_control_stmt->value.loop.end,
                                                               KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));
    return KEFIR_OK;
}
