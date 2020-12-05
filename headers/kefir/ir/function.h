#ifndef KEFIR_IR_FUNCTION_H_
#define KEFIR_IR_FUNCTION_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/ir/type.h"
#include "kefir/ir/instr.h"

typedef struct kefir_irfunction_decl {
    const char *identifier;
    struct kefir_ir_type params;
    struct kefir_ir_type result;
} kefir_irfunction_decl_t;

typedef struct kefir_irfunction {
    struct kefir_irfunction_decl declaration;
    struct kefir_irblock body;
} kefir_irfunction_t;

kefir_result_t kefir_irfunction_init(struct kefir_irfunction *, const char *);
kefir_result_t kefir_irfunction_alloc(struct kefir_mem *,
                                  const char *,
                                  kefir_size_t,
                                  kefir_size_t,
                                  kefir_size_t,
                                  struct kefir_irfunction *);
kefir_result_t kefir_irfunction_free(struct kefir_mem *, struct kefir_irfunction *);

#endif