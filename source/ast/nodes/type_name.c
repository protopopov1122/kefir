#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_type_name_visit, kefir_ast_type_name, type_name)

struct kefir_ast_node_base *ast_type_name_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_type_name_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_type_name *, node,
        base->self);
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_type_name_CLASS = {
    .type = KEFIR_AST_TYPE_NAME,
    .visit = ast_type_name_visit,
    .clone = ast_type_name_clone,
    .free = ast_type_name_free
};

struct kefir_ast_node_base *ast_type_name_clone(struct kefir_mem *mem,
                                              struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_type_name *, node,
        base->self);
    struct kefir_ast_type_name *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_name));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_type_name_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->type = node->type;
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_type_name *kefir_ast_new_type_name(struct kefir_mem *mem,
                                                const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    struct kefir_ast_type_name *type_name = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type_name));
    REQUIRE(type_name != NULL, NULL);
    type_name->base.klass = &AST_type_name_CLASS;
    type_name->base.self = type_name;
    kefir_result_t res = kefir_ast_node_properties_init(&type_name->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, type_name);
        return NULL;
    });
    type_name->type = type;
    return type_name;
}