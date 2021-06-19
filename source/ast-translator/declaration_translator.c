#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/initializer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_declaration(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                               struct kefir_irbuilder_block *builder,
                                               struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR builder block"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(
        node->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION && node->klass->type == KEFIR_AST_DECLARATION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected declaration AST node"));

    ASSIGN_DECL_CAST(struct kefir_ast_declaration *, declaration, node->self);
    REQUIRE(declaration->initializer != NULL, KEFIR_OK);

    REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder,
                                                  declaration->base.properties.declaration_props.identifier,
                                                  declaration->base.properties.declaration_props.scoped_id));
    REQUIRE_OK(kefir_ast_translate_initializer(mem, context, builder, declaration->base.properties.type,
                                               declaration->initializer));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
    return KEFIR_OK;
}