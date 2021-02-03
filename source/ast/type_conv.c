#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define ANY_OF(x, y, z) (KEFIR_AST_TYPE_SAME((x), (z)) || KEFIR_AST_TYPE_SAME((y), (z)))

const struct kefir_ast_type *kefir_ast_type_common_arithmetic(const struct kefir_ast_type *type1,
                                                          const struct kefir_ast_type *type2) {
    REQUIRE(type1->basic && type2->basic,
        NULL);
    if (KEFIR_AST_TYPE_SAME(type1, type2)) {
        return type1;
    }
    if (ANY_OF(type1, type2, kefir_ast_type_double())) {
        return kefir_ast_type_double();
    }
    if (ANY_OF(type1, type2, kefir_ast_type_float())) {
        return kefir_ast_type_float();
    }
    if (type1->basic_traits.signedness == type2->basic_traits.signedness) {
        if (type1->basic_traits.rank > type2->basic_traits.rank) {
            return type1;
        } else {
            return type2;
        }
    } else if (!type1->basic_traits.signedness &&
        type2->basic_traits.signedness) {
        if (type1->basic_traits.rank >= type2->basic_traits.rank) {
            return type1;
        } else {
            return type2;
        }
    } else {
        if (type1->basic_traits.rank <= type2->basic_traits.rank) {
            return type2;
        } else {
            return type1;
        }
    }
}