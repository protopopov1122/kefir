#include "kefir/ast/translator/typeconv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_typeconv(struct kefir_irbuilder_block *builder,
                                        const struct kefir_ast_type *original,
                                        const struct kefir_ast_type *destination) {
    if (original->tag == KEFIR_AST_TYPE_SCALAR_FLOAT &&
        destination->tag == KEFIR_AST_TYPE_SCALAR_DOUBLE) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_F32CF64, 0));
    } else if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(original) &&
        destination->tag == KEFIR_AST_TYPE_SCALAR_FLOAT) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INTCF32, 0));
    } else if (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(original) &&
        destination->tag == KEFIR_AST_TYPE_SCALAR_DOUBLE) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_INTCF64, 0));
    } else if (KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(original) &&
        destination->tag == KEFIR_AST_TYPE_SCALAR_FLOAT) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_UINTCF32, 0));
    } else if (KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(original) &&
        destination->tag == KEFIR_AST_TYPE_SCALAR_DOUBLE) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_UINTCF64, 0));
    } else if (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(original) &&
        KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(destination)) {
        return KEFIR_OK; // No conversion needed
    }
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected arithmetic types");
}