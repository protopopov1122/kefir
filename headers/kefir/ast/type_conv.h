#ifndef KEFIR_AST_TYPE_CONV_H_
#define KEFIR_AST_TYPE_CONV_H_

#include "kefir/ast/type.h"

const struct kefir_ast_type *kefir_ast_type_int_promotion(const struct kefir_ast_type_traits *,
                                                          const struct kefir_ast_type *);
const struct kefir_ast_type *kefir_ast_type_common_arithmetic(const struct kefir_ast_type_traits *,
                                                              const struct kefir_ast_type *,
                                                              const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_function_default_argument_promotion(const struct kefir_ast_type_traits *,
                                                                                const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_lvalue_conversion(const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_array_conversion(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                             const struct kefir_ast_type *);

const struct kefir_ast_type *kefir_ast_type_function_conversion(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                                const struct kefir_ast_type *);

typedef enum kefir_ast_type_conv_expression_wrapper_param {
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_NONE = 0u,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_LVALUE = 1u,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_ARRAY = 1u << 1,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_FUNCTION = 1u << 2,
    KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_ALL = (1u << 3) - 1u
} kefir_ast_type_conv_expression_wrapper_param_t;

const struct kefir_ast_type *kefir_ast_type_conv_expression_wrapper(struct kefir_mem *, struct kefir_ast_type_bundle *,
                                                                    const struct kefir_ast_type *, kefir_int_t);

#define KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, type_bundle, type)       \
    kefir_ast_type_conv_expression_wrapper((mem), (type_bundle), (type), \
                                           KEFIR_AST_TYPE_CONV_EXPRESSION_WRAPPER_PARAM_ALL)

const struct kefir_ast_type *kefir_ast_type_conv_adjust_function_parameter(struct kefir_mem *,
                                                                           struct kefir_ast_type_bundle *,
                                                                           const struct kefir_ast_type *);

#endif
