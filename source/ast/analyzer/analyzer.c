#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/ast/analyzer/nodes.h"

struct assign_param {
    struct kefir_mem *mem;
    struct kefir_ast_node_base *base;
    const struct kefir_ast_context *context;
};

static kefir_result_t visit_non_expression(const struct kefir_ast_visitor *visitor,
                                         const struct kefir_ast_node_base *base,
                                         void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to assign type to non-expression AST node");
}

static kefir_result_t visit_constant(const struct kefir_ast_visitor *visitor,
                                   const struct kefir_ast_constant *node,
                                   void *payload) {
    UNUSED(visitor);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST analyzer payload"));
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    return kefir_ast_analyze_constant_node(param->mem, param->context, node, param->base);
}

static kefir_result_t visit_identifier(const struct kefir_ast_visitor *visitor,
                                     const struct kefir_ast_identifier *node,
                                     void *payload) {
    UNUSED(visitor);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST analyzer payload"));
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    return kefir_ast_analyze_identifier_node(param->mem, param->context, node, param->base);
}

static kefir_result_t visit_unary_operation(const struct kefir_ast_visitor *visitor,
                                          const struct kefir_ast_unary_operation *node,
                                          void *payload) {
    UNUSED(visitor);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST analyzer payload"));
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    return kefir_ast_analyze_unary_operation_node(param->mem, param->context, node, param->base);
}

static kefir_result_t visit_binary_operation(const struct kefir_ast_visitor *visitor,
                                           const struct kefir_ast_binary_operation *node,
                                           void *payload) {
    UNUSED(visitor);
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST analyzer payload"));
    ASSIGN_DECL_CAST(struct assign_param *, param,
        payload);
    return kefir_ast_analyze_binary_operation_node(param->mem, param->context, node, param->base);
}

kefir_result_t kefir_ast_analyze_node(struct kefir_mem *mem,
                                  const struct kefir_ast_context *context,
                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translaction_context"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    struct assign_param param = {
        .mem = mem,
        .context = context,
        .base = base
    };
    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, visit_non_expression));
    visitor.constant = visit_constant;
    visitor.identifier = visit_identifier;
    visitor.unary_operation = visit_unary_operation;
    visitor.binary_operation = visit_binary_operation;
    return KEFIR_AST_NODE_VISIT(&visitor, base, &param);
}