#include "kefir/core/tree.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t child_node_remove(struct kefir_mem *mem,
                         struct kefir_list *list,
                         struct kefir_list_entry *entry,
                         void *payload) {
    UNUSED(list);
    UNUSED(payload);
    ASSIGN_DECL_CAST(struct kefir_tree_node *, node,
        entry->value);
    REQUIRE_OK(kefir_tree_free(mem, node));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

kefir_result_t kefir_tree_init(struct kefir_tree_node *root, void *value) {
    REQUIRE(root != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));
    root->value = value;
    root->parent = NULL;
    root->prev_sibling = NULL;
    root->next_sibling = NULL;
    REQUIRE_OK(kefir_list_init(&root->children));
    REQUIRE_OK(kefir_list_on_remove(&root->children, child_node_remove, NULL));
    root->removal_callback = NULL;
    root->removal_payload = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_tree_free(struct kefir_mem *mem, struct kefir_tree_node *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));

    if (node->removal_callback != NULL) {
        REQUIRE_OK(node->removal_callback(mem, node->value, node->removal_payload));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_tree_on_removal(struct kefir_tree_node *node,
                                 kefir_result_t (*callback)(struct kefir_mem *, void *, void *),
                                 void *payload) {
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));
    node->removal_callback = callback;
    node->removal_payload = payload;
    return KEFIR_OK;
}

kefir_result_t kefir_tree_insert_child(struct kefir_mem *mem, struct kefir_tree_node *node, void *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid tree node"));
    struct kefir_tree_node *child = KEFIR_MALLOC(mem, sizeof(struct kefir_tree_node));
    REQUIRE(child != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate tree node"));
    child->value = value;
    child->parent = node;
    child->removal_callback = node->removal_callback;
    child->removal_payload = node->removal_payload;
    kefir_result_t res = kefir_list_init(&child->children);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, child);
        return res;
    });
    res = kefir_list_on_remove(&child->children, child_node_remove, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &child->children);
        KEFIR_FREE(mem, child);
        return res;
    });
    child->prev_sibling = (struct kefir_tree_node *) kefir_list_tail(&node->children)->value;
    child->next_sibling = NULL;
    if (child->prev_sibling != NULL) {
        child->prev_sibling->next_sibling = child;
    }
    return KEFIR_OK;
}

struct kefir_tree_node *kefir_tree_first_child(const struct kefir_tree_node *node) {
    REQUIRE(node != NULL, NULL);
    return (struct kefir_tree_node *) kefir_list_head(&node->children)->value;
}

struct kefir_tree_node *kefir_tree_next_sibling(const struct kefir_tree_node *node) {
    REQUIRE(node != NULL, NULL);
    return node->next_sibling;
}

struct kefir_tree_node *kefir_tree_prev_sibling(const struct kefir_tree_node *node) {
    REQUIRE(node != NULL, NULL);
    return node->prev_sibling;
}