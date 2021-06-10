#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_function_call_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                    const struct kefir_ast_function_call *node,
                                                    struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function call"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context, node->function));
    const struct kefir_list_entry *iter;
    for (iter = kefir_list_head(&node->arguments); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, arg));
    }

    const struct kefir_ast_type *func_type =
        KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->type_bundle, node->function->properties.type);
    REQUIRE(
        func_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER && func_type->referenced_type->tag == KEFIR_AST_TYPE_FUNCTION,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected expression to be a pointer to function"));
    const struct kefir_ast_type *function_type = func_type->referenced_type;

    if (function_type->function_type.mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        const struct kefir_list_entry *arg_value_iter = kefir_list_head(&node->arguments);
        const struct kefir_list_entry *arg_type_iter = kefir_list_head(&function_type->function_type.parameters);
        for (; arg_value_iter != NULL && arg_type_iter != NULL;
             kefir_list_next(&arg_type_iter), kefir_list_next(&arg_value_iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter, arg_type_iter->value);
            ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg, arg_value_iter->value);
            REQUIRE_OK(
                kefir_ast_node_assignable(mem, context, arg, kefir_ast_unqualified_type(parameter->adjusted_type)));
        }

        if (arg_type_iter != NULL) {
            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, parameter, arg_type_iter->value);
            REQUIRE(parameter->adjusted_type->tag == KEFIR_AST_TYPE_VOID &&
                        kefir_ast_type_function_parameter_count(&function_type->function_type) == 1,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function call parameter count does not match prototype"));
        } else if (arg_value_iter != NULL) {
            REQUIRE(function_type->function_type.ellipsis,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function call parameter count does not match prototype"));
        }
    }

    const struct kefir_ast_type *return_type =
        kefir_ast_unqualified_type(func_type->referenced_type->function_type.return_type);
    REQUIRE(return_type->tag != KEFIR_AST_TYPE_ARRAY,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function cannot return array type"));
    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_EXPRESSION;
    base->properties.type = return_type;
    return KEFIR_OK;
}
