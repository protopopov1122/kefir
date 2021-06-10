#ifndef KEFIR_AST_NODE_INTERNAL_H_
#define KEFIR_AST_NODE_INTERNAL_H_

#include "kefir/core/basic-types.h"

#define NODE_VISIT_IMPL(identifier, type, handler)                                                             \
    kefir_result_t identifier(const struct kefir_ast_node_base *base, const struct kefir_ast_visitor *visitor, \
                              void *payload) {                                                                 \
        REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));           \
        REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node visitor"));     \
        ASSIGN_DECL_CAST(const struct type *, node, base->self);                                               \
        if (visitor->handler != NULL) {                                                                        \
            return visitor->handler(visitor, node, payload);                                                   \
        } else if (visitor->generic_handler != NULL) {                                                         \
            return visitor->generic_handler(visitor, base, payload);                                           \
        } else {                                                                                               \
            return KEFIR_OK;                                                                                   \
        }                                                                                                      \
    }

#endif
