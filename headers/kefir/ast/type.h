#ifndef KEFIR_AST_TYPE_H_
#define KEFIR_AST_TYPE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/list.h"
#include "kefir/core/symbol_table.h"

typedef struct kefir_ast_type kefir_ast_type_t;

typedef enum kefir_ast_type_tag {
    KEFIR_AST_TYPE_VOID,
    KEFIR_AST_TYPE_SCALAR_BOOL,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR,
    KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT,
    KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT,
    KEFIR_AST_TYPE_SCALAR_SIGNED_INT,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG,
    KEFIR_AST_TYPE_SCALAR_SIGNED_LONG,
    KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG,
    KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG,
    KEFIR_AST_TYPE_SCALAR_FLOAT,
    KEFIR_AST_TYPE_SCALAR_DOUBLE,
    KEFIR_AST_TYPE_SCALAR_POINTER,
    KEFIR_AST_TYPE_STRUCTURE,
    KEFIR_AST_TYPE_UNION,
    KEFIR_AST_TYPE_ARRAY,
    KEFIR_AST_TYPE_QUALIFIED
} kefir_ast_type_tag_t;

typedef struct kefir_ast_basic_type_traits {
    kefir_bool_t signedness;
    kefir_size_t rank;
} kefir_ast_basic_type_traits_t;

typedef struct kefir_ast_struct_field {
    const char *identifier;
    const struct kefir_ast_type *type;
    kefir_bool_t bitfield;
    kefir_size_t bitwidth;
} kefir_ast_struct_field_t;

typedef struct kefir_ast_struct_type {
    kefir_bool_t complete;
    const char *identifier;
    struct kefir_list fields;
} kefir_ast_struct_type_t;

typedef struct kefir_ast_array_type {
    const struct kefir_ast_type *element_type;
    kefir_bool_t bounded;
    kefir_size_t length;
} kefir_ast_array_type_t;

typedef struct kefir_ast_qualified_type {
    const struct kefir_ast_type *type;
    kefir_bool_t constant;
    kefir_bool_t restricted;
    kefir_bool_t volatile_type;
} kefir_ast_qualified_type_t;

typedef struct kefir_ast_type_ops {
    kefir_bool_t (*same)(const struct kefir_ast_type *, const struct kefir_ast_type *);
    kefir_result_t (*free)(struct kefir_mem *, const struct kefir_ast_type *);
} kefir_ast_type_ops_t;

typedef struct kefir_ast_type {
    kefir_ast_type_tag_t tag;
    kefir_bool_t basic;
    struct kefir_ast_type_ops ops;
    union {
        struct kefir_ast_basic_type_traits basic_traits;
        const struct kefir_ast_type *pointer_to;
        struct kefir_ast_struct_type structure;
        struct kefir_ast_array_type array;
        struct kefir_ast_qualified_type qualified;
    };
} kefir_ast_type_t;

#define SCALAR_TYPE(id) \
    const struct kefir_ast_type *kefir_ast_type_##id()
SCALAR_TYPE(void);
SCALAR_TYPE(bool);
SCALAR_TYPE(unsigned_char);
SCALAR_TYPE(signed_char);
SCALAR_TYPE(unsigned_short);
SCALAR_TYPE(signed_short);
SCALAR_TYPE(unsigned_int);
SCALAR_TYPE(signed_int);
SCALAR_TYPE(unsigned_long);
SCALAR_TYPE(signed_long);
SCALAR_TYPE(unsigned_long_long);
SCALAR_TYPE(signed_long_long);
SCALAR_TYPE(float);
SCALAR_TYPE(double);
#undef SCALAR_TYPE

typedef struct kefir_ast_type_repository {
    struct kefir_symbol_table symbols;
    struct kefir_list types;
} kefir_ast_type_repository_t;

const struct kefir_ast_type *kefir_ast_type_qualified(struct kefir_mem *,
                                                  struct kefir_ast_type_repository *,
                                                  const struct kefir_ast_type *,
                                                  kefir_bool_t,
                                                  kefir_bool_t,
                                                  kefir_bool_t);
const struct kefir_ast_type *kefir_ast_type_pointer(struct kefir_mem *,
                                                struct kefir_ast_type_repository *,
                                                const struct kefir_ast_type *);
const struct kefir_ast_type *kefir_ast_type_incomplete_structure(struct kefir_mem *,
                                                             struct kefir_ast_type_repository *,
                                                             const char *);
const struct kefir_ast_type *kefir_ast_type_incomplete_union(struct kefir_mem *,
                                                         struct kefir_ast_type_repository *,
                                                         const char *);
const struct kefir_ast_type *kefir_ast_type_structure(struct kefir_mem *,
                                                  struct kefir_ast_type_repository *,
                                                  const char *,
                                                  struct kefir_ast_struct_type **);
const struct kefir_ast_type *kefir_ast_type_union(struct kefir_mem *,
                                              struct kefir_ast_type_repository *,
                                              const char *,
                                              struct kefir_ast_struct_type **);
const struct kefir_ast_type *kefir_ast_type_unbounded_array(struct kefir_mem *,
                                                        struct kefir_ast_type_repository *,
                                                        const struct kefir_ast_type *);
const struct kefir_ast_type *kefir_ast_type_array(struct kefir_mem *,
                                              struct kefir_ast_type_repository *,
                                              const struct kefir_ast_type *,
                                              kefir_size_t);

kefir_result_t kefir_ast_type_repository_init(struct kefir_ast_type_repository *);
kefir_result_t kefir_ast_type_repository_free(struct kefir_mem *, struct kefir_ast_type_repository *);

#define KEFIR_AST_TYPE_SAME(type1, type2) ((type1)->ops.same((type1), (type2)))
#define KEFIR_AST_TYPE_FREE(mem, type) ((type)->ops.free((mem), (type)))

#define KEFIR_AST_TYPE_IS_SIGNED_INTEGER(base) \
    ((base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_INT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG)
#define KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(base) \
    ((base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG)
#define KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(base) \
    (KEFIR_AST_TYPE_IS_SIGNED_INTEGER(base) || \
        KEFIR_AST_TYPE_IS_UNSIGNED_INTEGER(base))
#define KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(base) \
    (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(base) || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_FLOAT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_DOUBLE)

#endif