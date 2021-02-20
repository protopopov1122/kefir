#ifndef KEFIR_AST_TYPE_QUALIFIED_H_
#define KEFIR_AST_TYPE_QUALIFIED_H_

#include "kefir/ast/type/base.h"

typedef struct kefir_ast_qualified_type {
    const struct kefir_ast_type *type;
    struct kefir_ast_type_qualification qualification;
} kefir_ast_qualified_type_t;

const struct kefir_ast_type *kefir_ast_type_qualified(struct kefir_mem *,
                                                  struct kefir_ast_type_storage *,
                                                  const struct kefir_ast_type *,
                                                  struct kefir_ast_type_qualification);
                                                  
const struct kefir_ast_type *kefir_ast_unqualified_type(const struct kefir_ast_type *);

#endif