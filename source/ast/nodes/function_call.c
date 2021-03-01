#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_function_call_visit, kefir_ast_function_call, function_call)

struct kefir_ast_node_base *ast_function_call_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_function_call_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_function_call *, node,
        base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->function));
    REQUIRE_OK(kefir_list_free(mem, &node->arguments));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_FUNCTION_CALL_CLASS = {
    .type = KEFIR_AST_FUNCTION_CALL,
    .visit = ast_function_call_visit,
    .clone = ast_function_call_clone,
    .free = ast_function_call_free
};

static kefir_result_t function_call_argument_free(struct kefir_mem *mem,
                                                struct kefir_list *list,
                                                struct kefir_list_entry *entry,
                                                void *payload) {
    UNUSED(list);
    UNUSED(payload);
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node,
        entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    return KEFIR_OK;
}

struct kefir_ast_node_base *ast_function_call_clone(struct kefir_mem *mem,
                                                  struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_function_call *, node,
        base->self);
    struct kefir_ast_function_call *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_function_call));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_FUNCTION_CALL_CLASS;
    clone->base.self = clone;
    clone->base.expression_type = node->base.expression_type;
    kefir_result_t res = kefir_list_init(&clone->arguments);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    res = kefir_list_on_remove(&clone->arguments, function_call_argument_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &clone->arguments);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->function = KEFIR_AST_NODE_CLONE(mem, node->function);
    REQUIRE_ELSE(clone->function != NULL, {
        kefir_list_free(mem, &clone->arguments);
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    for (const struct kefir_list_entry *iter = kefir_list_head(&node->arguments);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, arg,
            iter->value);
        struct kefir_ast_node_base *arg_clone = KEFIR_AST_NODE_CLONE(mem, arg);
        REQUIRE_ELSE(arg_clone != NULL, {
            KEFIR_AST_NODE_FREE(mem, clone->function);
            kefir_list_free(mem, &clone->arguments);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
        res = kefir_list_insert_after(mem, &clone->arguments, kefir_list_tail(&clone->arguments), arg_clone);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, clone->function);
            kefir_list_free(mem, &clone->arguments);
            KEFIR_FREE(mem, clone);
            return NULL;
        });
    }
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_function_call *kefir_ast_new_function_call(struct kefir_mem *mem,
                                                        struct kefir_ast_node_base *function) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(function != NULL, NULL);
    struct kefir_ast_function_call *function_call = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_function_call));
    REQUIRE(function_call != NULL, NULL);
    function_call->base.klass = &AST_FUNCTION_CALL_CLASS;
    function_call->base.self = function_call;
    function_call->base.expression_type = NULL;
    function_call->function = function;
    kefir_result_t res = kefir_list_init(&function_call->arguments);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, function_call);
        return NULL;
    });
    res = kefir_list_on_remove(&function_call->arguments, function_call_argument_free, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &function_call->arguments);
        KEFIR_FREE(mem, function_call);
        return NULL;
    });
    return function_call;
}

kefir_result_t kefir_ast_function_call_append(struct kefir_mem *mem,
                                          struct kefir_ast_function_call *call,
                                          struct kefir_ast_node_base *arg) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(call != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function call"));
    REQUIRE(arg != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid function parameter AST node"));
    REQUIRE_OK(kefir_list_insert_after(mem, &call->arguments, kefir_list_tail(&call->arguments), arg));
    return KEFIR_OK;
}