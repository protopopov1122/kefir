#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_conditional_operator_node(struct kefir_mem *mem,
                                                             struct kefir_ast_translator_context *context,
                                                             struct kefir_irbuilder_block *builder,
                                                             const struct kefir_ast_conditional_operator *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST conditional operator node"));

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->condition, builder, context));
    kefir_size_t jmp1Index = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_BRANCH, 0));
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->expr2, builder, context));
    kefir_size_t jmp2Index = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_JMP, 0));
    KEFIR_IRBUILDER_BLOCK_INSTR_AT(builder, jmp1Index)->arg.i64 = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);
    REQUIRE_OK(kefir_ast_translate_expression(mem, node->expr1, builder, context));
    KEFIR_IRBUILDER_BLOCK_INSTR_AT(builder, jmp2Index)->arg.i64 = KEFIR_IRBUILDER_BLOCK_CURRENT_INDEX(builder);

    return KEFIR_OK;
}
