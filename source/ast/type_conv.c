#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

const struct kefir_ast_type *kefir_ast_type_int_promotion(const struct kefir_ast_type_traits *type_traits,
                                                      const struct kefir_ast_type *type) {
    REQUIRE(type != NULL, NULL);
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type) || type->tag == KEFIR_AST_TYPE_SCALAR_BOOL, NULL);
    const struct kefir_ast_type *SIGNED_INT = kefir_ast_type_signed_int();
    const struct kefir_ast_type *UNSIGNED_INT = kefir_ast_type_unsigned_int();
    if (type->basic_type.rank < SIGNED_INT->basic_type.rank) {
        kefir_bool_t fits = false;
        if (type_traits->integral_type_fits(type_traits, type, SIGNED_INT, &fits) == KEFIR_OK && fits) {
            return SIGNED_INT;
        } else if (type_traits->integral_type_fits(type_traits, type, UNSIGNED_INT, &fits) == KEFIR_OK && fits) {
            return UNSIGNED_INT;
        } else {
            return NULL;
        }
    }
    return type;
}

#define ANY_OF(x, y, z) (KEFIR_AST_TYPE_SAME((x), (z)) || KEFIR_AST_TYPE_SAME((y), (z)))

const struct kefir_ast_type *kefir_ast_type_common_arithmetic(const struct kefir_ast_type_traits *type_traits,
                                                          const struct kefir_ast_type *type1,
                                                          const struct kefir_ast_type *type2) {
    REQUIRE(type1->basic && type2->basic, NULL);
    if (ANY_OF(type1, type2, kefir_ast_type_double())) {
        return kefir_ast_type_double();
    }
    if (ANY_OF(type1, type2, kefir_ast_type_float())) {
        return kefir_ast_type_float();
    }
    type1 = kefir_ast_type_int_promotion(type_traits, type1);
    type2 = kefir_ast_type_int_promotion(type_traits, type2);
    REQUIRE(type1 != NULL, NULL);
    REQUIRE(type2 != NULL, NULL);
    if (KEFIR_AST_TYPE_SAME(type1, type2)) {
        return type1;
    }
    if (type1->basic_type.signedness == type2->basic_type.signedness) {
        if (type1->basic_type.rank > type2->basic_type.rank) {
            return type1;
        } else {
            return type2;
        }
    } 
    kefir_bool_t fits = false;
    if (!type1->basic_type.signedness) {
        if (type1->basic_type.rank >= type2->basic_type.rank) {
            return type1;
        }
    } else if (type2->basic_type.rank >= type1->basic_type.rank) {
        return type2;
    }
    if (type1->basic_type.signedness) {
        if (type_traits->integral_type_fits(type_traits, type2, type1, &fits) == KEFIR_OK && fits) {
            return type1;
        }
    } else if (type_traits->integral_type_fits(type_traits, type1, type2, &fits) == KEFIR_OK && fits) {
        return type2;
    }
    if (type1->basic_type.signedness) {
        return kefir_ast_type_flip_integer_singedness(type1);
    } else {
        return kefir_ast_type_flip_integer_singedness(type2);
    }
}