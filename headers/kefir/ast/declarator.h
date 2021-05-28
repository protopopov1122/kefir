#ifndef KEFIR_AST_DECLARATOR_H_
#define KEFIR_AST_DECLARATOR_H_

#include "kefir/ast/declarator_specifier.h"

typedef struct kefir_ast_declarator kefir_ast_declarator_t; // Forward declaration

typedef struct kefir_ast_declarator_pointer {
    struct kefir_ast_type_qualifier_list type_qualifiers;
    struct kefir_ast_declarator *declarator;
} kefir_ast_declarator_pointer_t;

typedef enum kefir_ast_declarator_array_type {
    KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED,
    KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED,
    KEFIR_AST_DECLARATOR_ARRAY_BOUNDED
} kefir_ast_declarator_array_type_t;

typedef struct kefir_ast_declarator_array {
    struct kefir_ast_type_qualifier_list type_qualifiers;
    kefir_bool_t static_array;
    kefir_ast_declarator_array_type_t type;
    struct kefir_ast_node_base *length;
    struct kefir_ast_declarator *declarator;
} kefir_ast_declarator_array_t;

typedef struct kefir_ast_declarator_function {
    struct kefir_list parameters;
    kefir_bool_t ellipsis;
    struct kefir_ast_declarator *declarator;
} kefir_ast_declarator_function_t;

typedef struct kefir_ast_declarator {
    kefir_ast_declarator_class_t klass;
    union {
        const char *identifier;
        struct kefir_ast_declarator_pointer pointer;
        struct kefir_ast_declarator_array array;
        struct kefir_ast_declarator_function function;
    };
} kefir_ast_declarator_t;

struct kefir_ast_declarator *kefir_ast_declarator_identifier(struct kefir_mem *, struct kefir_symbol_table *, const char *);
struct kefir_ast_declarator *kefir_ast_declarator_pointer(struct kefir_mem *, struct kefir_ast_declarator *);
struct kefir_ast_declarator *kefir_ast_declarator_array(struct kefir_mem *,
                                                    kefir_ast_declarator_array_type_t,
                                                    struct kefir_ast_node_base *,
                                                    struct kefir_ast_declarator *);
struct kefir_ast_declarator *kefir_ast_declarator_function(struct kefir_mem *, struct kefir_ast_declarator *);
struct kefir_ast_declarator *kefir_ast_declarator_clone(struct kefir_mem *,
                                                    const struct kefir_ast_declarator *);
kefir_result_t kefir_ast_declarator_free(struct kefir_mem *, struct kefir_ast_declarator *);

kefir_result_t kefir_ast_declarator_is_abstract(struct kefir_ast_declarator *, kefir_bool_t *);

#endif
