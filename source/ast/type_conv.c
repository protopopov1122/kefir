/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

const struct kefir_ast_type *kefir_ast_type_int_promotion(const struct kefir_ast_type_traits *type_traits,
                                                          const struct kefir_ast_type *type) {
    REQUIRE(type != NULL, NULL);
    REQUIRE(KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(type), NULL);

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

    kefir_bool_t type1_sign, type2_sign;
    REQUIRE(kefir_ast_type_is_signed(type_traits, type1, &type1_sign) == KEFIR_OK, NULL);
    REQUIRE(kefir_ast_type_is_signed(type_traits, type2, &type2_sign) == KEFIR_OK, NULL);

    if (type1_sign == type2_sign) {
        if (type1->basic_type.rank > type2->basic_type.rank) {
            return type1;
        } else {
            return type2;
        }
    }
    kefir_bool_t fits = false;
    if (!type1_sign) {
        if (type1->basic_type.rank >= type2->basic_type.rank) {
            return type1;
        }
    } else if (type2->basic_type.rank >= type1->basic_type.rank) {
        return type2;
    }
    if (type1_sign) {
        if (type_traits->integral_type_fits(type_traits, type2, type1, &fits) == KEFIR_OK && fits) {
            return type1;
        }
    } else if (type_traits->integral_type_fits(type_traits, type1, type2, &fits) == KEFIR_OK && fits) {
        return type2;
    }
    if (type1_sign) {
        return kefir_ast_type_flip_integer_singedness(type_traits, type1);
    } else {
        return kefir_ast_type_flip_integer_singedness(type_traits, type2);
    }
}

const struct kefir_ast_type *kefir_ast_type_function_default_argument_promotion(
    const struct kefir_ast_type_traits *type_traits, const struct kefir_ast_type *type) {
    if (KEFIR_AST_TYPE_IS_NONENUM_INTEGRAL_TYPE(type)) {
        return kefir_ast_type_int_promotion(type_traits, type);
    } else if (type->tag == KEFIR_AST_TYPE_SCALAR_FLOAT) {
        return kefir_ast_type_double();
    } else {
        return type;
    }
}

const struct kefir_ast_type *kefir_ast_type_lvalue_conversion(const struct kefir_ast_type *type) {
    REQUIRE(type != NULL, NULL);
    return kefir_ast_unqualified_type(type);
}

const struct kefir_ast_type *kefir_ast_type_array_conversion(struct kefir_mem *mem,
                                                             struct kefir_ast_type_bundle *type_bundle,
                                                             const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    REQUIRE(type->tag == KEFIR_AST_TYPE_ARRAY, type);
    return kefir_ast_type_pointer(mem, type_bundle, type->array_type.element_type);
}

const struct kefir_ast_type *kefir_ast_type_function_conversion(struct kefir_mem *mem,
                                                                struct kefir_ast_type_bundle *type_bundle,
                                                                const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    REQUIRE(type->tag == KEFIR_AST_TYPE_FUNCTION, type);
    return kefir_ast_type_pointer(mem, type_bundle, type);
}

const struct kefir_ast_type *kefir_ast_type_conv_expression_wrapper(struct kefir_mem *mem,
                                                                    struct kefir_ast_type_bundle *type_bundle,
                                                                    const struct kefir_ast_type *type,
                                                                    kefir_int_t param) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);
    if ((param & KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_LVALUE) != 0) {
        type = kefir_ast_type_lvalue_conversion(type);
    }
    if ((param & KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_ARRAY) != 0) {
        type = kefir_ast_type_array_conversion(mem, type_bundle, type);
    }
    if ((param & KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_FUNCTION) != 0) {
        type = kefir_ast_type_function_conversion(mem, type_bundle, type);
    }
    return type;
}

const struct kefir_ast_type *kefir_ast_type_conv_adjust_function_parameter(struct kefir_mem *mem,
                                                                           struct kefir_ast_type_bundle *type_bundle,
                                                                           const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type_bundle != NULL, NULL);
    REQUIRE(type != NULL, NULL);

    switch (type->tag) {
        case KEFIR_AST_TYPE_FUNCTION:
            return kefir_ast_type_pointer(mem, type_bundle, type);

        case KEFIR_AST_TYPE_ARRAY: {
            const struct kefir_ast_type *adjusted =
                kefir_ast_type_pointer(mem, type_bundle, type->array_type.element_type);
            if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&type->array_type.qualifications)) {
                adjusted = kefir_ast_type_qualified(mem, type_bundle, adjusted, type->array_type.qualifications);
            }
            return adjusted;
        }

        default:
            return type;
    }
}
