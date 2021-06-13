#ifndef KEFIR_AST_NODE_BASE_H_
#define KEFIR_AST_NODE_BASE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/list.h"
#include "kefir/ast/base.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/type.h"
#include "kefir/ast/temporaries.h"

typedef struct kefir_ast_visitor kefir_ast_visitor_t;
typedef struct kefir_ast_visitor kefir_ast_visitor_t;

typedef struct kefir_ast_node_class {
    kefir_ast_node_type_t type;
    kefir_result_t (*visit)(const struct kefir_ast_node_base *, const struct kefir_ast_visitor *, void *);
    struct kefir_ast_node_base *(*clone)(struct kefir_mem *, struct kefir_ast_node_base *);
    kefir_result_t (*free)(struct kefir_mem *, struct kefir_ast_node_base *);
} kefir_ast_node_class_t;

typedef struct kefir_ast_node_properties {
    kefir_ast_node_category_t category;
    const struct kefir_ast_type *type;
    struct {
        kefir_bool_t lvalue;
        kefir_bool_t constant_expression;
        kefir_bool_t addressable;
        kefir_bool_t bitfield;
        struct {
            const char *content;
            kefir_size_t length;
        } string_literal;
        struct kefir_ast_temporary_identifier temporary;
    } expression_props;
    struct {
        kefir_ast_scoped_identifier_storage_t storage;
        kefir_ast_function_specifier_t function;
        const char *identifier;
        kefir_size_t alignment;
        kefir_bool_t static_assertion;
    } declaration_props;

    struct {
        struct kefir_ast_flow_control_point *flow_control_point;
        struct kefir_ast_flow_control_statement *flow_control_statement;
    } statement_props;
} kefir_ast_node_properties_t;

typedef struct kefir_ast_node_base {
    const struct kefir_ast_node_class *klass;
    void *self;
    struct kefir_ast_node_properties properties;
} kefir_ast_node_base_t;

#define KEFIR_AST_NODE_STRUCT(id, content) \
    typedef struct id {                    \
        struct kefir_ast_node_base base;   \
        struct content;                    \
    } id##_t

#define KEFIR_AST_NODE_BASE(node) (&node->base)
#define KEFIR_AST_NODE_VISIT(visitor, base, payload) ((base)->klass->visit((base), (visitor), (payload)))
#define KEFIR_AST_NODE_CLONE(mem, base) ((base) != NULL ? (base)->klass->clone((mem), (base)) : NULL)
#define KEFIR_AST_NODE_FREE(mem, base) ((base)->klass->free((mem), (base)))

#define KEFIR_AST_VISITOR_METHOD(id, type) \
    kefir_result_t (*id)(const struct kefir_ast_visitor *, const struct type *, void *)

kefir_result_t kefir_ast_visitor_init(struct kefir_ast_visitor *,
                                      KEFIR_AST_VISITOR_METHOD(method, kefir_ast_node_base));

kefir_result_t kefir_ast_node_properties_init(struct kefir_ast_node_properties *);

kefir_result_t kefir_ast_node_properties_clone(struct kefir_ast_node_properties *,
                                               const struct kefir_ast_node_properties *);

#endif
