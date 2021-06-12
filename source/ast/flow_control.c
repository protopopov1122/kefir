#include "kefir/ast/flow_control.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include <string.h>

static kefir_result_t flow_control_statement_free(struct kefir_mem *mem, void *node, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_OK);

    ASSIGN_DECL_CAST(struct kefir_ast_flow_control_statement *, statement, node);
    if (statement->cleanup.callback != NULL) {
        statement->cleanup.callback(mem, statement, statement->cleanup.payload);
        statement->cleanup.callback = NULL;
        statement->cleanup.payload = NULL;
    }
    statement->parent = NULL;
    statement->payload.cleanup = NULL;
    statement->payload.ptr = NULL;
    KEFIR_FREE(mem, statement);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_init(struct kefir_ast_flow_control_tree *tree) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control tree"));
    REQUIRE_OK(kefir_tree_init(&tree->root, NULL));
    REQUIRE_OK(kefir_tree_on_removal(&tree->root, flow_control_statement_free, NULL));
    tree->current = &tree->root;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_free(struct kefir_mem *mem, struct kefir_ast_flow_control_tree *tree) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control tree"));

    REQUIRE_OK(kefir_tree_free(mem, &tree->root));
    tree->current = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_push(struct kefir_mem *mem, struct kefir_ast_flow_control_tree *tree,
                                                kefir_ast_flow_control_statement_type_t type,
                                                const struct kefir_ast_flow_control_statement **statement) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control tree"));

    const struct kefir_ast_flow_control_statement *parent = NULL;
    REQUIRE_OK(kefir_ast_flow_control_tree_top(tree, &parent));
    struct kefir_ast_flow_control_statement *stmt = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_flow_control_statement));
    REQUIRE(stmt != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST flow control statement"));
    stmt->type = type;
    stmt->parent = parent;
    stmt->cleanup.callback = NULL;
    stmt->cleanup.payload = NULL;
    stmt->payload.cleanup = &stmt->cleanup;
    stmt->payload.ptr = &stmt->payload.content[0];
    memset(&stmt->payload.content[0], 0, KEFIR_AST_FLOW_CONTROL_STATEMENT_PAYLOAD_SIZE);

    kefir_result_t res = kefir_tree_insert_child(mem, tree->current, stmt, &tree->current);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, stmt);
        return res;
    });

    ASSIGN_PTR(statement, stmt);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_pop(struct kefir_ast_flow_control_tree *tree) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control tree"));
    REQUIRE(tree->current->parent != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot pop flow control tree top-level statement"));

    tree->current = tree->current->parent;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_top(struct kefir_ast_flow_control_tree *tree,
                                               const struct kefir_ast_flow_control_statement **stmt) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control tree"));
    REQUIRE(stmt != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST flow control tree statement"));

    ASSIGN_DECL_CAST(struct kefir_ast_flow_control_statement *, statement, tree->current->value);
    ASSIGN_PTR(stmt, statement);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_flow_control_tree_traverse(
    struct kefir_ast_flow_control_tree *tree,
    kefir_result_t (*callback)(const struct kefir_ast_flow_control_statement *, void *, kefir_bool_t *), void *payload,
    const struct kefir_ast_flow_control_statement **stmt) {
    REQUIRE(tree != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST flow control tree"));
    REQUIRE(callback != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid traversal callback"));
    REQUIRE(stmt != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST flow control tree statement"));

    const struct kefir_ast_flow_control_statement *current = NULL;
    REQUIRE_OK(kefir_ast_flow_control_tree_top(tree, &current));

    do {
        kefir_bool_t match = false;
        REQUIRE_OK(callback(current, payload, &match));
        if (match) {
            ASSIGN_PTR(stmt, current);
            return KEFIR_OK;
        } else {
            current = current->parent;
        }
    } while (current != NULL);

    return KEFIR_NOT_FOUND;
}
