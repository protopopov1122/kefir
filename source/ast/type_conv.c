#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

const struct kefir_ast_type *kefir_ast_type_int_promotion(const struct kefir_ast_basic_types *basic_types,
                                                      const struct kefir_ast_type *type) {
    REQUIRE(type != NULL, NULL);
    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(type) || type->tag == KEFIR_AST_TYPE_SCALAR_BOOL, NULL);
    const struct kefir_ast_type *SIGNED_INT = KEFIR_AST_BASIC_TYPE_SIGNED_INT(basic_types);
    const struct kefir_ast_type *UNSIGNED_INT = KEFIR_AST_BASIC_TYPE_UNSIGNED_INT(basic_types);
    if (type->basic_traits.rank < SIGNED_INT->basic_traits.rank) {
        if (type->basic_traits.fit_rank < SIGNED_INT->basic_traits.fit_rank) {
            return SIGNED_INT;
        } else if (type->basic_traits.signedness && type->basic_traits.fit_rank == SIGNED_INT->basic_traits.fit_rank) {
            return SIGNED_INT;
        } else if (!type->basic_traits.signedness && type->basic_traits.fit_rank <= UNSIGNED_INT->basic_traits.fit_rank) {
            return UNSIGNED_INT;
        } else {
            return NULL;
        }
    }
    return type;
}

#define ANY_OF(x, y, z) (KEFIR_AST_TYPE_SAME((x), (z)) || KEFIR_AST_TYPE_SAME((y), (z)))

const struct kefir_ast_type *kefir_ast_type_common_arithmetic(const struct kefir_ast_basic_types *basic_types,
                                                          const struct kefir_ast_type *type1,
                                                          const struct kefir_ast_type *type2) {
    REQUIRE(type1->basic && type2->basic, NULL);
    if (ANY_OF(type1, type2, KEFIR_AST_BASIC_TYPE_DOUBLE(basic_types))) {
        return KEFIR_AST_BASIC_TYPE_DOUBLE(basic_types);
    }
    if (ANY_OF(type1, type2, KEFIR_AST_BASIC_TYPE_FLOAT(basic_types))) {
        return KEFIR_AST_BASIC_TYPE_FLOAT(basic_types);
    }
    type1 = kefir_ast_type_int_promotion(basic_types, type1);
    type2 = kefir_ast_type_int_promotion(basic_types, type2);
    REQUIRE(type1 != NULL, NULL);
    REQUIRE(type2 != NULL, NULL);
    if (KEFIR_AST_TYPE_SAME(type1, type2)) {
        return type1;
    }
    if (type1->basic_traits.signedness == type2->basic_traits.signedness) {
        if (type1->basic_traits.rank > type2->basic_traits.rank) {
            return type1;
        } else {
            return type2;
        }
    } 
    if (!type1->basic_traits.signedness) {
        if (type1->basic_traits.rank >= type2->basic_traits.rank) {
            return type1;
        }
    } else if (type2->basic_traits.rank >= type1->basic_traits.rank) {
        return type2;
    }
    if (type1->basic_traits.signedness) {
        if (type1->basic_traits.fit_rank > type2->basic_traits.fit_rank) {
            return type1;
        }
    } else if (type2->basic_traits.fit_rank > type1->basic_traits.fit_rank) {
        return type2;
    }
    if (type1->basic_traits.signedness) {
        return kefir_ast_type_flip_integer_singedness(basic_types, type1);
    } else {
        return kefir_ast_type_flip_integer_singedness(basic_types, type2);
    }
}