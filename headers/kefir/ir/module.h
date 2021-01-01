#ifndef KEFIR_IR_MODULE_H_
#define KEFIR_IR_MODULE_H_

#include "kefir/core/mem.h"
#include "kefir/core/list.h"
#include "kefir/core/hashtree.h"
#include "kefir/ir/function.h"

typedef struct kefir_ir_module {
    struct kefir_hashtree symbols;
    struct kefir_list types;
    struct kefir_hashtree function_declarations;
    struct kefir_list global_symbols;
    struct kefir_list external_symbols;
} kefir_ir_module_t;

kefir_result_t kefir_ir_module_alloc(struct kefir_mem *, struct kefir_ir_module *);
kefir_result_t kefir_ir_module_free(struct kefir_mem *,
                                struct kefir_ir_module *);

const char *kefir_ir_module_symbol(struct kefir_mem *,
                                 struct kefir_ir_module *,
                                 const char *);
struct kefir_ir_type *kefir_ir_module_new_type(struct kefir_mem *,
                                           struct kefir_ir_module *,
                                           kefir_size_t);

struct kefir_ir_function_decl * kefir_ir_module_new_function_declaration(struct kefir_mem *,
                                                                     struct kefir_ir_module *,
                                                                     const char *,
                                                                     struct kefir_ir_type *,
                                                                     struct kefir_ir_type *);

kefir_result_t kefir_ir_module_declare_global(struct kefir_mem *,
                                          struct kefir_ir_module *,
                                          const char *);

kefir_result_t kefir_ir_module_declare_external(struct kefir_mem *,
                                            struct kefir_ir_module *,
                                            const char *);

#endif