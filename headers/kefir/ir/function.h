#ifndef KEFIR_IR_FUNCTION_H_
#define KEFIR_IR_FUNCTION_H_

#include <stdbool.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"
#include "kefir/ir/type.h"
#include "kefir/ir/instr.h"

typedef struct kefir_ir_function_decl {
    kefir_id_t id;
    const char *name;
    struct kefir_ir_type *params;
    bool vararg;
    struct kefir_ir_type *result;
} kefir_ir_function_decl_t;

typedef struct kefir_ir_function {
    struct kefir_ir_function_decl *declaration;
    struct kefir_ir_type *locals;
    struct kefir_irblock body;
} kefir_ir_function_t;

kefir_result_t kefir_ir_function_decl_alloc(struct kefir_mem *,
                                       kefir_id_t,
                                       const char *,
                                       struct kefir_ir_type *,
                                       bool,
                                       struct kefir_ir_type *,
                                       struct kefir_ir_function_decl *);
kefir_result_t kefir_ir_function_decl_free(struct kefir_mem *,
                                      struct kefir_ir_function_decl *);

kefir_result_t kefir_ir_function_alloc(struct kefir_mem *,
                                  struct kefir_ir_function_decl *,
                                  struct kefir_ir_type *,
                                  kefir_size_t,
                                  struct kefir_ir_function *);
kefir_result_t kefir_ir_function_free(struct kefir_mem *, struct kefir_ir_function *);

#endif