#include "kefir/ast/initializer.h"
#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t list_entry_removal(struct kefir_mem *mem,
                                       struct kefir_list *list,
                                       struct kefir_list_entry *entry,
                                       void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, list_entry,
        entry->value);
    if (list_entry->designator != NULL) {
        REQUIRE_OK(kefir_ast_designator_free(mem, list_entry->designator));
        list_entry->designator = NULL;
    }
    REQUIRE_OK(kefir_ast_initializer_free(mem, list_entry->value));
    KEFIR_FREE(mem, list_entry);
    return KEFIR_OK;
}

struct kefir_ast_initializer *kefir_ast_new_expression_initializer(struct kefir_mem *mem,
                                                           struct kefir_ast_node_base *expr) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(expr != NULL, NULL);

    struct kefir_ast_initializer *initializer = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer));
    REQUIRE(initializer != NULL, NULL);
    initializer->type = KEFIR_AST_INITIALIZER_EXPRESSION;
    initializer->expression = expr;
    return initializer;
}

struct kefir_ast_initializer *kefir_ast_new_list_initializer(struct kefir_mem *mem) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_initializer *initializer = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer));
    REQUIRE(initializer != NULL, NULL);
    initializer->type = KEFIR_AST_INITIALIZER_LIST;
    kefir_result_t res = kefir_ast_initializer_list_init(&initializer->list);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, initializer);
        return NULL;
    });
    return initializer;
}

kefir_result_t kefir_ast_initializer_free(struct kefir_mem *mem,
                                      struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer"));
    
    switch (initializer->type) {
        case KEFIR_AST_INITIALIZER_EXPRESSION:
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, initializer->expression));
            break;

        case KEFIR_AST_INITIALIZER_LIST:
            REQUIRE_OK(kefir_ast_initializer_list_free(mem, &initializer->list));
            break;
    }
    KEFIR_FREE(mem, initializer);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_initializer_list_init(struct kefir_ast_initializer_list *list) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer list"));
    REQUIRE_OK(kefir_list_init(&list->initializers));
    REQUIRE_OK(kefir_list_on_remove(&list->initializers, list_entry_removal, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_initializer_list_free(struct kefir_mem *mem,
                                           struct kefir_ast_initializer_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer list"));
    REQUIRE_OK(kefir_list_free(mem, &list->initializers));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_initializer_list_append(struct kefir_mem *mem,
                                             struct kefir_ast_initializer_list *list,
                                             struct kefir_ast_designator *designator,
                                             struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer list"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer"));
    
    struct kefir_ast_initializer_list_entry *entry = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_initializer_list_entry));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST initializer list entry"));
    entry->designator = designator;
    entry->value = initializer;
    kefir_result_t res = kefir_list_insert_after(mem, &list->initializers, kefir_list_tail(&list->initializers), entry);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, entry);
        return res;
    });
    return KEFIR_OK;
}