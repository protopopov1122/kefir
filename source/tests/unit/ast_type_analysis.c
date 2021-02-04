#include "kefir/test/unit_test.h"
#include "kefir/ast/type_conv.h"

#define ASSERT_TYPE_TAG(type, _tag) \
    do { \
        ASSERT((type) != NULL); \
        ASSERT((type)->tag == (_tag)); \
    } while (0)

DEFINE_CASE(ast_type_analysis_integer_promotion1, "AST Type analysis - integer promotion")
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_bool()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_signed_char()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_unsigned_char()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_signed_short()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_unsigned_short()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_signed_int()), KEFIR_AST_TYPE_SCALAR_SIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_unsigned_int()), KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_signed_long()), KEFIR_AST_TYPE_SCALAR_SIGNED_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_unsigned_long()), KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_signed_long_long()), KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG);
    ASSERT_TYPE_TAG(kefir_ast_type_int_promotion(kefir_ast_type_unsigned_long_long()), KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG);
    ASSERT(kefir_ast_type_int_promotion(kefir_ast_type_void()) == NULL);
    ASSERT(kefir_ast_type_int_promotion(kefir_ast_type_float()) == NULL);
    ASSERT(kefir_ast_type_int_promotion(kefir_ast_type_double()) == NULL);
END_CASE