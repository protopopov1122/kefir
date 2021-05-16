#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_constant_visit, kefir_ast_constant, constant)

struct kefir_ast_node_base *ast_constant_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_constant_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_constant *, node,
        base->self);
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_CONSTANT_CLASS = {
    .type = KEFIR_AST_CONSTANT,
    .visit = ast_constant_visit,
    .clone = ast_constant_clone,
    .free = ast_constant_free
};

struct kefir_ast_node_base *ast_constant_clone(struct kefir_mem *mem,
                                             struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_constant *, node,
        base->self);
    struct kefir_ast_constant *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_CONSTANT_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });
    clone->type = node->type;
    clone->value = node->value;
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_constant *kefir_ast_new_constant_bool(struct kefir_mem *mem, kefir_bool_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_BOOL_CONSTANT;
    constant->value.boolean = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_char(struct kefir_mem *mem, kefir_char_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_CHAR_CONSTANT;
    constant->value.character = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_int(struct kefir_mem *mem, kefir_int_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_INT_CONSTANT;
    constant->value.integer = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_uint(struct kefir_mem *mem, kefir_uint_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_UINT_CONSTANT;
    constant->value.uinteger = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_long(struct kefir_mem *mem, kefir_long_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_LONG_CONSTANT;
    constant->value.long_integer = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_ulong(struct kefir_mem *mem, kefir_ulong_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_ULONG_CONSTANT;
    constant->value.ulong_integer = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_long_long(struct kefir_mem *mem, kefir_long_long_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_LONG_LONG_CONSTANT;
    constant->value.long_long = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_ulong_long(struct kefir_mem *mem, kefir_ulong_long_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_ULONG_LONG_CONSTANT;
    constant->value.ulong_long = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_float(struct kefir_mem *mem, kefir_float32_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_FLOAT_CONSTANT;
    constant->value.float32 = value;
    return constant;
}

struct kefir_ast_constant *kefir_ast_new_constant_double(struct kefir_mem *mem, kefir_float64_t value) {
    REQUIRE(mem != NULL, NULL);
    struct kefir_ast_constant *constant = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_constant));
    REQUIRE(constant != NULL, NULL);
    constant->base.klass = &AST_CONSTANT_CLASS;
    constant->base.self = constant;
    kefir_result_t res = kefir_ast_node_properties_init(&constant->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, constant);
        return NULL;
    });
    constant->type = KEFIR_AST_DOUBLE_CONSTANT;
    constant->value.float64 = value;
    return constant;
}
