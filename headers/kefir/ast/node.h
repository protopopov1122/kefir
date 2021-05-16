#ifndef KEFIR_AST_NODE_H_
#define KEFIR_AST_NODE_H_

#include "kefir/ast/node_base.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/initializer.h"

KEFIR_AST_NODE_STRUCT(kefir_ast_constant, {
    kefir_ast_constant_type_t type;
    union {
        kefir_bool_t boolean;
        kefir_char_t character;
        kefir_int_t integer;
        kefir_uint_t uinteger;
        kefir_long_t long_integer;
        kefir_ulong_t ulong_integer;
        kefir_long_long_t long_long;
        kefir_ulong_long_t ulong_long;
        kefir_float32_t float32;
        kefir_float64_t float64;
    } value;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_identifier, {
    const char *identifier;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_string_literal, {
    char *literal;
    kefir_size_t length;
});

typedef struct kefir_ast_generic_selection_assoc {
    const struct kefir_ast_type *type;
    struct kefir_ast_node_base *expr;
} kefir_ast_generic_selection_assoc_t;

KEFIR_AST_NODE_STRUCT(kefir_ast_generic_selection, {
    struct kefir_ast_node_base *control;
    struct kefir_list associations;
    struct kefir_ast_node_base *default_assoc;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_type_name, {
    const struct kefir_ast_type *type;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_compound_literal, {
    const struct kefir_ast_type *type;
    struct kefir_ast_initializer *initializer;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_cast_operator, {
    const struct kefir_ast_type *type;
    struct kefir_ast_node_base *expr;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_array_subscript, {
   struct kefir_ast_node_base *array;
   struct kefir_ast_node_base *subscript; 
});

KEFIR_AST_NODE_STRUCT(kefir_ast_function_call, {
   struct kefir_ast_node_base *function;
   struct kefir_list arguments;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_struct_member, {
   struct kefir_ast_node_base *structure;
   const char *member; 
});

KEFIR_AST_NODE_STRUCT(kefir_ast_unary_operation, {
    kefir_ast_unary_operation_type_t type;
    struct kefir_ast_node_base *arg;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_binary_operation, {
    kefir_ast_binary_operation_type_t type;
    struct kefir_ast_node_base *arg1;
    struct kefir_ast_node_base *arg2;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_conditional_operator, {
    struct kefir_ast_node_base *condition;
    struct kefir_ast_node_base *expr1;
    struct kefir_ast_node_base *expr2;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_assignment_operator, {
    kefir_ast_assignment_operation_t operation;
    struct kefir_ast_node_base *target;
    struct kefir_ast_node_base *value;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_comma_operator, {
    struct kefir_list expressions;
});

struct kefir_ast_constant *kefir_ast_new_constant_bool(struct kefir_mem *, kefir_bool_t);
struct kefir_ast_constant *kefir_ast_new_constant_char(struct kefir_mem *, kefir_char_t);
struct kefir_ast_constant *kefir_ast_new_constant_int(struct kefir_mem *, kefir_int_t);
struct kefir_ast_constant *kefir_ast_new_constant_uint(struct kefir_mem *, kefir_uint_t);
struct kefir_ast_constant *kefir_ast_new_constant_long(struct kefir_mem *, kefir_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_ulong(struct kefir_mem *, kefir_ulong_t);
struct kefir_ast_constant *kefir_ast_new_constant_long_long(struct kefir_mem *, kefir_long_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_ulong_long(struct kefir_mem *, kefir_ulong_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_float(struct kefir_mem *, kefir_float32_t);
struct kefir_ast_constant *kefir_ast_new_constant_double(struct kefir_mem *, kefir_float64_t);
struct kefir_ast_identifier *kefir_ast_new_identifier(struct kefir_mem *,
                                                  struct kefir_symbol_table *,
                                                  const char *);

struct kefir_ast_string_literal *kefir_ast_new_string_literal(struct kefir_mem *,
                                                          const char *,
                                                          kefir_size_t);

#define KEFIR_AST_MAKE_STRING_LITERAL(_mem, _string) \
    (kefir_ast_new_string_literal((_mem), (_string), strlen((_string)) + 1))

struct kefir_ast_generic_selection *kefir_ast_new_generic_selection(struct kefir_mem *,
                                                                struct kefir_ast_node_base *);

struct kefir_ast_type_name *kefir_ast_new_type_name(struct kefir_mem *,
                                                const struct kefir_ast_type *);

struct kefir_ast_compound_literal *kefir_ast_new_compound_literal(struct kefir_mem *,
                                                              const struct kefir_ast_type *);

struct kefir_ast_cast_operator *kefir_ast_new_cast_operator(struct kefir_mem *,
                                                        const struct kefir_ast_type *,
                                                        struct kefir_ast_node_base *);

struct kefir_ast_array_subscript *kefir_ast_new_array_subscript(struct kefir_mem *,
                                                            struct kefir_ast_node_base *,
                                                            struct kefir_ast_node_base *);

struct kefir_ast_function_call *kefir_ast_new_function_call(struct kefir_mem *,
                                                        struct kefir_ast_node_base *);

struct kefir_ast_struct_member *kefir_ast_new_struct_member(struct kefir_mem *,
                                                        struct kefir_symbol_table *,
                                                        struct kefir_ast_node_base *,
                                                        const char *);
struct kefir_ast_struct_member *kefir_ast_new_struct_indirect_member(struct kefir_mem *,
                                                                 struct kefir_symbol_table *,
                                                                 struct kefir_ast_node_base *,
                                                                 const char *);
struct kefir_ast_unary_operation *kefir_ast_new_unary_operation(struct kefir_mem *,
                                                            kefir_ast_unary_operation_type_t,
                                                            struct kefir_ast_node_base *);
struct kefir_ast_binary_operation *kefir_ast_new_binary_operation(struct kefir_mem *,
                                                              kefir_ast_binary_operation_type_t,
                                                              struct kefir_ast_node_base *,
                                                              struct kefir_ast_node_base *);

struct kefir_ast_conditional_operator *kefir_ast_new_conditional_operator(struct kefir_mem *,
                                                                        struct kefir_ast_node_base *,
                                                                        struct kefir_ast_node_base *,
                                                                        struct kefir_ast_node_base *);

struct kefir_ast_assignment_operator *kefir_ast_new_simple_assignment(struct kefir_mem *,
                                                               struct kefir_ast_node_base *,
                                                               struct kefir_ast_node_base *);

struct kefir_ast_assignment_operator *kefir_ast_new_compound_assignment(struct kefir_mem *,
                                                                 kefir_ast_assignment_operation_t,
                                                                 struct kefir_ast_node_base *,
                                                                 struct kefir_ast_node_base *);

struct kefir_ast_comma_operator *kefir_ast_new_comma_operator(struct kefir_mem *);

typedef struct kefir_ast_visitor {
    KEFIR_AST_VISITOR_METHOD(generic_handler, kefir_ast_node_base);
    KEFIR_AST_VISITOR_METHOD(constant, kefir_ast_constant);
    KEFIR_AST_VISITOR_METHOD(identifier, kefir_ast_identifier);
    KEFIR_AST_VISITOR_METHOD(string_literal, kefir_ast_string_literal);
    KEFIR_AST_VISITOR_METHOD(generic_selection, kefir_ast_generic_selection);
    KEFIR_AST_VISITOR_METHOD(type_name, kefir_ast_type_name);
    KEFIR_AST_VISITOR_METHOD(cast_operator, kefir_ast_cast_operator);
    KEFIR_AST_VISITOR_METHOD(array_subscript, kefir_ast_array_subscript);
    KEFIR_AST_VISITOR_METHOD(function_call, kefir_ast_function_call);
    KEFIR_AST_VISITOR_METHOD(struct_member, kefir_ast_struct_member);
    KEFIR_AST_VISITOR_METHOD(struct_indirect_member, kefir_ast_struct_member);
    KEFIR_AST_VISITOR_METHOD(unary_operation, kefir_ast_unary_operation);
    KEFIR_AST_VISITOR_METHOD(binary_operation, kefir_ast_binary_operation);
    KEFIR_AST_VISITOR_METHOD(conditional_operator, kefir_ast_conditional_operator);
    KEFIR_AST_VISITOR_METHOD(assignment_operator, kefir_ast_assignment_operator);
    KEFIR_AST_VISITOR_METHOD(comma_operator, kefir_ast_comma_operator);
    KEFIR_AST_VISITOR_METHOD(compound_literal, kefir_ast_compound_literal);
} kefir_ast_visitor_t;

#define KEFIR_AST_NODE_INTERNAL_DEF
#include "kefir/ast/node_helpers.h"
#undef KEFIR_AST_NODE_INTERNAL_DEF

#endif
