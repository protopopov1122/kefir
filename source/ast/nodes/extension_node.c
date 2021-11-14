#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_extension_node_visit, kefir_ast_extension_node, extension_node)

struct kefir_ast_node_base *ast_extension_node_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_extension_node_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_extension_node *, node, base->self);
    REQUIRE_OK(node->klass->free(mem, node));
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_EXTENSION_NODE_CLASS = {.type = KEFIR_AST_EXTENSION_NODE,
                                                              .visit = ast_extension_node_visit,
                                                              .clone = ast_extension_node_clone,
                                                              .free = ast_extension_node_free};

struct kefir_ast_node_base *ast_extension_node_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);

    ASSIGN_DECL_CAST(struct kefir_ast_extension_node *, node, base->self);
    struct kefir_ast_extension_node *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_extension_node));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_EXTENSION_NODE_CLASS;
    clone->base.self = clone;
    clone->base.source_location = base->source_location;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    res = node->klass->clone(mem, clone, node);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_extension_node *kefir_ast_new_extension_node(struct kefir_mem *mem,
                                                              const struct kefir_ast_extension_node_class *klass,
                                                              void *payload) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(klass != NULL, NULL);

    struct kefir_ast_extension_node *extension_node = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_extension_node));
    REQUIRE(extension_node != NULL, NULL);
    extension_node->base.klass = &AST_EXTENSION_NODE_CLASS;
    extension_node->base.self = extension_node;
    kefir_result_t res = kefir_ast_node_properties_init(&extension_node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, extension_node);
        return NULL;
    });
    res = kefir_source_location_empty(&extension_node->base.source_location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, extension_node);
        return NULL;
    });

    extension_node->klass = klass;
    extension_node->payload = payload;
    return extension_node;
}
