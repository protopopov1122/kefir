#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_visitor_init(struct kefir_ast_visitor *visitor,
                                  kefir_result_t (*generic)(const struct kefir_ast_visitor *, const struct kefir_ast_node_base *, void *)) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST visitor"));
    *visitor = (const struct kefir_ast_visitor){0};
    visitor->generic_handler = generic;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_node_properties_init(struct kefir_ast_node_properties *props) {
    REQUIRE(props != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node properties pointer"));
    props->category = KEFIR_AST_NODE_CATEGORY_UNKNOWN;
    props->type = NULL;
    props->expression_props.constant_expression = false;
    props->expression_props.lvalue = false;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_node_properties_clone(struct kefir_ast_node_properties *dst_props,
                                           const struct kefir_ast_node_properties *src_props) {
    REQUIRE(dst_props != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node properties destination pointer"));
    REQUIRE(src_props != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node properties source pointer"));
    dst_props->category = src_props->category;
    dst_props->type = src_props->type;
    return KEFIR_OK;
}