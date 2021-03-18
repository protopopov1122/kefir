#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_struct_member_node(struct kefir_mem *mem,
                                                const struct kefir_ast_context *context,
                                                const struct kefir_ast_struct_member *node,
                                                struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST struct member"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->structure));

    REQUIRE(node->structure->properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Structure shall be an expression"));

    const struct kefir_ast_type *struct_type = node->structure->properties.type;
    const struct kefir_ast_type_qualification *qualification = NULL;
    if (node->base.klass->type == KEFIR_AST_STRUCTURE_INDIRECT_MEMBER) {
        struct_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, struct_type);
        REQUIRE(struct_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected pointer type on the left side"));
        struct_type = struct_type->referenced_type;
    }
    if (struct_type->tag == KEFIR_AST_TYPE_QUALIFIED) {
        REQUIRE(struct_type->qualified_type.type->tag == KEFIR_AST_TYPE_STRUCTURE ||
            struct_type->qualified_type.type->tag == KEFIR_AST_TYPE_UNION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected qualified structure or union type on the left side"));
        qualification = &struct_type->qualified_type.qualification;
        struct_type = struct_type->qualified_type.type;
    } else {
        REQUIRE(struct_type->tag == KEFIR_AST_TYPE_STRUCTURE ||
            struct_type->tag == KEFIR_AST_TYPE_UNION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected structure or union type on the left side"));
    }

    const struct kefir_ast_struct_field *field = NULL;
    REQUIRE_OK(kefir_ast_struct_type_get_field(&struct_type->structure_type, node->member, &field));
    const struct kefir_ast_type *type = field->type;
    if (qualification != NULL) {
        type = kefir_ast_type_qualified(mem, context->type_bundle, type, *qualification);
    }

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = type;
    base->properties.expression_props.lvalue = true;
    base->properties.expression_props.addressable = !field->bitfield;
    base->properties.expression_props.bitfield = field->bitfield;
    return KEFIR_OK;
}