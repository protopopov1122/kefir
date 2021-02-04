#ifndef KEFIR_AST_TYPE_CONV_H_
#define KEFIR_AST_TYPE_CONV_H_

#include "kefir/ast/type.h"

const struct kefir_ast_type *kefir_ast_type_int_promotion(const struct kefir_ast_type *);
const struct kefir_ast_type *kefir_ast_type_common_arithmetic(const struct kefir_ast_type *,
                                                              const struct kefir_ast_type *);

#endif