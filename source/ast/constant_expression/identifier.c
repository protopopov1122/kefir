#include "kefir/ast/constant_expression_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_evaluate_identifier_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_identifier *node,
                                                  struct kefir_ast_constant_expression_value *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant node"));
    REQUIRE(value != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression value pointer"));
    REQUIRE(node->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected constant expression AST node"));
    REQUIRE(node->base.properties.expression_props.constant_expression,
            KEFIR_SET_ERROR(KEFIR_NOT_CONSTANT, "Expected constant expression AST node"));

    const struct kefir_ast_scoped_identifier *scoped_id = NULL;
    REQUIRE_OK(context->resolve_ordinary_identifier(context, node->identifier, &scoped_id));
    switch (scoped_id->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT: {
            *value = scoped_id->enum_constant.value->value;
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
            REQUIRE(scoped_id->type->tag == KEFIR_AST_TYPE_ARRAY &&
                        (scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
                         scoped_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC),
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Identifier in a constant expression shall refer either to an "
                                                         "array of static lifetime or to a function"));
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
            value->pointer.type = KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER;
            value->pointer.base.literal = node->identifier;
            value->pointer.offset = 0;
            value->pointer.pointer_node = KEFIR_AST_NODE_BASE(node);
            value->pointer.scoped_id = scoped_id;
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
            value->klass = KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS;
            value->pointer.type = KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER;
            value->pointer.base.literal = node->identifier;
            value->pointer.offset = 0;
            value->pointer.pointer_node = KEFIR_AST_NODE_BASE(node);
            value->pointer.scoped_id = scoped_id;
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
        case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
            return KEFIR_SET_ERROR(KEFIR_NOT_CONSTANT, "Expected object or constant identifier");
    }
    return KEFIR_OK;
}
