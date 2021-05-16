#ifndef KEFIR_AST_TEMPORARIES_H_
#define KEFIR_AST_TEMPORARIES_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ast/base.h"

typedef struct kefir_ast_temporary_identifier {
    kefir_id_t identifier;
    kefir_id_t field;
} kefir_ast_temporary_identifier_t;

typedef struct kefir_ast_context_temporaries {
    const struct kefir_ast_type *type;
    struct kefir_ast_struct_type *temporaries;
    struct kefir_ast_temporary_identifier current;
} kefir_ast_context_temporaries_t;

kefir_bool_t kefir_ast_temporaries_init(struct kefir_mem *,
                                    struct kefir_ast_type_bundle *,
                                    struct kefir_ast_context_temporaries *);
kefir_result_t kefir_ast_temporaries_next_block(struct kefir_ast_context_temporaries *);
kefir_result_t kefir_ast_temporaries_new_temporary(struct kefir_mem *,
                                               const struct kefir_ast_context *,
                                               const struct kefir_ast_type *,
                                               struct kefir_ast_temporary_identifier *);

#endif
