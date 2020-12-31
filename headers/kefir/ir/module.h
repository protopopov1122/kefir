#ifndef KEFIR_IR_MODULE_H_
#define KEFIR_IR_MODULE_H_

#include "kefir/core/mem.h"
#include "kefir/core/list.h"
#include "kefir/ir/function.h"

typedef struct kefir_ir_module {
    struct kefir_list type_head;
} kefir_ir_module_t;

kefir_result_t kefir_ir_module_init(struct kefir_ir_module *);
kefir_result_t kefir_ir_module_free(struct kefir_mem *,
                                struct kefir_ir_module *);
struct kefir_ir_type *kefir_ir_module_new_type(struct kefir_mem *,
                                                  struct kefir_ir_module *,
                                                  kefir_size_t);

#endif