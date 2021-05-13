#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_struct_member_node(struct kefir_mem *mem,
                                                  struct kefir_ast_translator_context *context,
                                                  struct kefir_irbuilder_block *builder,
                                                  const struct kefir_ast_struct_member *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST struct member node"));
    
    REQUIRE_OK(kefir_ast_translate_struct_member_lvalue(mem, context, builder, node));
    REQUIRE_OK(kefir_ast_translator_resolve_node_value(mem, context, builder, KEFIR_AST_NODE_BASE(node)));
    return KEFIR_OK;
}