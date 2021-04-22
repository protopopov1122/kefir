#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/translator_impl.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/ir/builder.h"

struct translator_param {
    struct kefir_mem *mem;
    struct kefir_ast_translator_context *context;
    struct kefir_irbuilder_block *builder;
};

static kefir_result_t translate_not_impl(const struct kefir_ast_visitor *visitor,
                                       const struct kefir_ast_node_base *base,
                                       void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate non-expression AST node");
}

#define TRANSLATE_NODE(_id, _type) \
    static kefir_result_t translate_##_id(const struct kefir_ast_visitor *visitor, \
                                        const _type *node, \
                                        void *payload) { \
        REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor")); \
        REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node")); \
        REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload")); \
        ASSIGN_DECL_CAST(struct translator_param *, param, \
            payload); \
        REQUIRE_OK(kefir_ast_translate_##_id##_node(param->mem, param->context, param->builder, node)); \
        return KEFIR_OK; \
    }

TRANSLATE_NODE(constant, struct kefir_ast_constant)
TRANSLATE_NODE(identifier, struct kefir_ast_identifier)
TRANSLATE_NODE(generic_selection, struct kefir_ast_generic_selection)
TRANSLATE_NODE(string_literal, struct kefir_ast_string_literal)
TRANSLATE_NODE(compound_literal, struct kefir_ast_compound_literal)
TRANSLATE_NODE(array_subscript, struct kefir_ast_array_subscript)
TRANSLATE_NODE(struct_member, struct kefir_ast_struct_member)
TRANSLATE_NODE(function_call, struct kefir_ast_function_call)
TRANSLATE_NODE(cast_operator, struct kefir_ast_cast_operator)
TRANSLATE_NODE(unary_operation, struct kefir_ast_unary_operation)
TRANSLATE_NODE(binary_operation, struct kefir_ast_binary_operation)
TRANSLATE_NODE(comma_operator, struct kefir_ast_comma_operator)
TRANSLATE_NODE(conditional_operator, struct kefir_ast_conditional_operator)
TRANSLATE_NODE(assignment_operator, struct kefir_ast_assignment_operator)
#undef TRANSLATE_NODE

kefir_result_t kefir_ast_translate_expression(struct kefir_mem *mem,
                                          const struct kefir_ast_node_base *base,
                                          struct kefir_irbuilder_block *builder,
                                          struct kefir_ast_translator_context *context) {
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, translate_not_impl));
    visitor.constant = translate_constant;
    visitor.identifier = translate_identifier;
    visitor.generic_selection = translate_generic_selection;
    visitor.string_literal = translate_string_literal;
    visitor.compound_literal = translate_compound_literal;
    visitor.array_subscript = translate_array_subscript;
    visitor.struct_member = translate_struct_member;
    visitor.struct_indirect_member = translate_struct_member;
    visitor.function_call = translate_function_call;
    visitor.cast_operator = translate_cast_operator;
    visitor.unary_operation = translate_unary_operation;
    visitor.binary_operation = translate_binary_operation;
    visitor.comma_operator = translate_comma_operator;
    visitor.conditional_operator = translate_conditional_operator;
    visitor.assignment_operator = translate_assignment_operator;

    struct translator_param param = {
        .mem = mem,
        .builder = builder,
        .context = context
    };
    return KEFIR_AST_NODE_VISIT(&visitor, base, &param);
}
