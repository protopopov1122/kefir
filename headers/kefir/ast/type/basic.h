#ifndef KEFIR_AST_TYPE_BASIC_H_
#define KEFIR_AST_TYPE_BASIC_H_

#include "kefir/ast/type/base.h"

typedef struct kefir_ast_basic_type_properties {
    kefir_size_t rank;
} kefir_ast_basic_type_properties_t;

#define SCALAR_TYPE(id) \
    const struct kefir_ast_type *kefir_ast_type_##id()
SCALAR_TYPE(void);
SCALAR_TYPE(bool);
SCALAR_TYPE(char);
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

#define KEFIR_AST_TYPE_IS_CHARACTER(base) \
    ((base)->tag == KEFIR_AST_TYPE_SCALAR_CHAR || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR)
#define KEFIR_INTERNAL_AST_TYPE_IS_SIGNED_INTEGER(base) \
    ((base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_INT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG)
#define KEFIR_INTERNAL_AST_TYPE_IS_UNSIGNED_INTEGER(base) \
    ((base)->tag == KEFIR_AST_TYPE_SCALAR_BOOL || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG)
#define KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(base) \
    (KEFIR_AST_TYPE_IS_CHARACTER(base) || \
        KEFIR_INTERNAL_AST_TYPE_IS_SIGNED_INTEGER(base) || \
        KEFIR_INTERNAL_AST_TYPE_IS_UNSIGNED_INTEGER(base))
#define KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(base) \
    (KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(base) || \
        (base)->tag == KEFIR_AST_TYPE_ENUMERATION)
#define KEFIR_AST_TYPE_IS_FLOATING_POINT(base) \
    ((base)->tag == KEFIR_AST_TYPE_SCALAR_FLOAT || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_DOUBLE)
#define KEFIR_AST_TYPE_IS_REAL_TYPE(base) \
    (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(base) || \
        KEFIR_AST_TYPE_IS_FLOATING_POINT(base))
#define KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(base) \
    (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(base) || \
        KEFIR_AST_TYPE_IS_FLOATING_POINT(base))
#define KEFIR_AST_TYPE_IS_SCALAR_TYPE(base) \
    (KEFIR_AST_TYPE_IS_ARITHMETIC_TYPE(base) || \
        (base)->tag == KEFIR_AST_TYPE_SCALAR_POINTER)

const struct kefir_ast_type *kefir_ast_type_flip_integer_singedness(const struct kefir_ast_type_traits *,
                                                                const struct kefir_ast_type *);

kefir_result_t kefir_ast_type_is_signed(const struct kefir_ast_type_traits *,
                                    const struct kefir_ast_type *,
                                    kefir_bool_t *);

#endif
