#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_for_statement_node(struct kefir_mem *mem,
                                                      struct kefir_ast_translator_context *context,
                                                      struct kefir_irbuilder_block *builder,
                                                      const struct kefir_ast_for_statement *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST for statement node"));

    if (node->init != NULL && node->init->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION) {
        REQUIRE_OK(kefir_ast_translate_declaration(mem, node->init, builder, context));
    } else if (node->init != NULL) {
        REQUIRE(node->init->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Expected AST for statement first clause to be either declaration or expression"));

        const struct kefir_ast_type *clause1_type = kefir_ast_translator_normalize_type(node->init->properties.type);
        REQUIRE(clause1_type != NULL,
                KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to obtain normalized expression type"));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->init, builder, context));
        if (clause1_type->tag != KEFIR_AST_TYPE_VOID) {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    }

    struct kefir_ast_flow_control_statement *flow_control_stmt =
        node->base.properties.statement_props.flow_control_statement;

    kefir_size_t begin = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);

    if (node->controlling_expr != NULL) {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->controlling_expr, builder, context));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BNOT, 0));
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
        REQUIRE_OK(kefir_ast_translator_flow_control_point_reference(
            mem, flow_control_stmt->value.loop.end, builder->block, KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder) - 1));
    }

    REQUIRE_OK(kefir_ast_translate_statement(mem, node->body, builder, context));

    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(mem, flow_control_stmt->value.loop.continuation,
                                                               KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));
    if (node->tail != NULL) {
        const struct kefir_ast_type *tail_type = kefir_ast_translator_normalize_type(node->tail->properties.type);
        REQUIRE(tail_type != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Unable to obtain normalized expression type"));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->tail, builder, context));
        if (tail_type->tag != KEFIR_AST_TYPE_VOID) {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    }

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_JMP, begin));
    REQUIRE_OK(kefir_ast_translator_flow_control_point_resolve(mem, flow_control_stmt->value.loop.end,
                                                               KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder)));

    return KEFIR_OK;
}
