#include <stdio.h>
#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/temporaries.h"
#include "kefir/ast-translator/initializer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_compound_literal_node(struct kefir_mem *mem,
                                                     struct kefir_ast_translator_context *context,
                                                     struct kefir_irbuilder_block *builder,
                                                     const struct kefir_ast_compound_literal *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST compound literal node"));
    
    REQUIRE_OK(kefir_ast_translator_fetch_temporary(mem, context, builder,
        &node->base.properties.expression_props.temporary));
    REQUIRE_OK(kefir_ast_translate_initializer(mem, context, builder, node->base.properties.type, node->initializer));
    return KEFIR_OK;
}