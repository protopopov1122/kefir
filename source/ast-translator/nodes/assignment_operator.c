#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_assignment_operator_node(struct kefir_mem *mem,
                                                        struct kefir_ast_translator_context *context,
                                                        struct kefir_irbuilder_block *builder,
                                                        const struct kefir_ast_assignment_operator *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant node"));

    REQUIRE_OK(kefir_ast_translate_lvalue(mem, context, builder, node->target));
    switch (node->operation) {
        case KEFIR_AST_ASSIGNMENT_SIMPLE:
            REQUIRE_OK(kefir_ast_translate_expression(mem, node->value, builder, context));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 2));
            REQUIRE_OK(kefir_ast_translator_store_value(mem, node->base.properties.type, context, builder));
            break;
            
        case KEFIR_AST_ASSIGNMENT_MULTIPLY:
        case KEFIR_AST_ASSIGNMENT_DIVIDE:
        case KEFIR_AST_ASSIGNMENT_MODULO:
        case KEFIR_AST_ASSIGNMENT_ADD:
        case KEFIR_AST_ASSIGNMENT_SUBTRACT:
        case KEFIR_AST_ASSIGNMENT_SHIFT_LEFT:
        case KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT:
        case KEFIR_AST_ASSIGNMENT_BITWISE_AND:
        case KEFIR_AST_ASSIGNMENT_BITWISE_OR:
        case KEFIR_AST_ASSIGNMENT_BITWISE_XOR:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Compound assignments are not implemented yet");
    }
    return KEFIR_OK;
}