#ifndef KEFIR_AST_TYPE_POINTER_H_
#define KEFIR_AST_TYPE_POINTER_H_

#include "kefir/ast/type/base.h"

const struct kefir_ast_type *kefir_ast_type_pointer(struct kefir_mem *,
                                                struct kefir_ast_type_repository *,
                                                const struct kefir_ast_type *);

#endif