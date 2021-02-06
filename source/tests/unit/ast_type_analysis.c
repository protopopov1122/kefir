#include "kefir/test/unit_test.h"
#include "kefir/ast/type_conv.h"

#define ASSERT_TYPE_TAG(type, _tag) \
    do { \
        ASSERT((type) != NULL); \
        ASSERT((type)->tag == (_tag)); \
    } while (0)

DEFINE_CASE(ast_type_analysis_integer_promotion1, "AST Type analysis - integer promotion")
    const struct kefir_ast_basic_types *basic_types = kefir_ast_default_basic_types();
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_BOOL(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_SIGNED_CHAR(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_UNSIGNED_CHAR(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_SIGNED_SHORT(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_UNSIGNED_SHORT(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_SIGNED_INT(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_UNSIGNED_INT(basic_types)), KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_SIGNED_LONG(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG(basic_types)), KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_SIGNED_LONG_LONG(basic_types)), KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG_LONG(basic_types)), KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG);
    ASSERT(kefir_ast_type_int_promotion(basic_types, kefir_ast_type_void()) == NULL);
    ASSERT(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_FLOAT(basic_types)) == NULL);
    ASSERT(kefir_ast_type_int_promotion(basic_types,
        KEFIR_AST_BASIC_TYPE_DOUBLE(basic_types)) == NULL);
END_CASE

DEFINE_CASE(ast_type_analysis_arithmetic_conversion1, "AST Type analysis - arithmetic conversion #1")
    const struct kefir_ast_basic_types *basic_types = kefir_ast_default_basic_types();
    const struct kefir_ast_type *TYPES[] = {
        KEFIR_AST_BASIC_TYPE_BOOL(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_CHAR(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_CHAR(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_SHORT(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_SHORT(basic_types)
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES_LEN; j++) {
            ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], TYPES[j]),
                KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
        }
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], KEFIR_AST_BASIC_TYPE_SIGNED_INT(basic_types)),
            KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], KEFIR_AST_BASIC_TYPE_UNSIGNED_INT(basic_types)),
            KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], KEFIR_AST_BASIC_TYPE_SIGNED_LONG(basic_types)),
            KEFIR_AST_TYPE_SCALAR_SIGNED_LONG);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG(basic_types)),
            KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], KEFIR_AST_BASIC_TYPE_SIGNED_LONG_LONG(basic_types)),
            KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG);
        ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG_LONG(basic_types)),
            KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG);
    }
END_CASE

DEFINE_CASE(ast_type_analysis_arithmetic_conversion2, "AST Type analysis - arithmetic conversion #2")
    const struct kefir_ast_basic_types *basic_types = kefir_ast_default_basic_types();
    const struct kefir_ast_type *TYPES[] = {
        KEFIR_AST_BASIC_TYPE_BOOL(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_CHAR(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_CHAR(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_SHORT(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_SHORT(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_INT(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_INT(basic_types)
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);
    const struct kefir_ast_type *TYPES2[] = {
        KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_LONG_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_FLOAT(basic_types),
        KEFIR_AST_BASIC_TYPE_DOUBLE(basic_types)
    };
    const kefir_size_t TYPES2_LEN = sizeof(TYPES2) / sizeof(TYPES2[0]);
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES2_LEN; j++) {
            ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], TYPES2[j]),
                TYPES2[j]->tag);
        }
    }
END_CASE

DEFINE_CASE(ast_type_analysis_arithmetic_conversion3, "AST Type analysis - arithmetic conversion #3")
    const struct kefir_ast_basic_types *basic_types = kefir_ast_default_basic_types();
    const struct kefir_ast_type *TYPES[] = {
        KEFIR_AST_BASIC_TYPE_BOOL(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_CHAR(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_CHAR(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_SHORT(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_SHORT(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_INT(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_INT(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_UNSIGNED_LONG_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_SIGNED_LONG_LONG(basic_types),
        KEFIR_AST_BASIC_TYPE_FLOAT(basic_types)
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);
    const struct kefir_ast_type *TYPES2[] = {
        KEFIR_AST_BASIC_TYPE_FLOAT(basic_types),
        KEFIR_AST_BASIC_TYPE_DOUBLE(basic_types)
    };
    const kefir_size_t TYPES2_LEN = sizeof(TYPES2) / sizeof(TYPES2[0]);
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES2_LEN; j++) {
            ASSERT_TYPE_TAG(kefir_ast_type_common_arithmetic(basic_types, TYPES[i], TYPES2[j]),
                TYPES2[j]->tag);
        }
    }
END_CASE