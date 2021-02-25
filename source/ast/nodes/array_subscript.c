#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_array_subscript_visit, kefir_ast_array_subscript, array_subscript)

kefir_result_t ast_array_subscript_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_array_subscript *, node,
        base->self);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->array));
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node->subscript));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_ARRAY_SUBSCRIPT_CLASS = {
    .type = KEFIR_AST_ARRAY_SUBSCRIPT,
    .visit = ast_array_subscript_visit,
    .free = ast_array_subscript_free
};

struct kefir_ast_array_subscript *kefir_ast_new_array_subscript(struct kefir_mem *mem,
                                                            struct kefir_ast_node_base *array,
                                                            struct kefir_ast_node_base *subscript) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(array != NULL, NULL);
    REQUIRE(subscript != NULL, NULL);
    struct kefir_ast_array_subscript *array_subscript = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_array_subscript));
    REQUIRE(array_subscript != NULL, NULL);
    array_subscript->base.klass = &AST_ARRAY_SUBSCRIPT_CLASS;
    array_subscript->base.self = array_subscript;
    array_subscript->base.expression_type = NULL;
    array_subscript->array = array;
    array_subscript->subscript = subscript;
    return array_subscript;
}