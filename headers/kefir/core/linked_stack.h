#ifndef KEFIR_CORE_LINKED_STACK_H_
#define KEFIR_CORE_LINKED_STACK_H_

#include "kefir/core/list.h"

kefir_result_t kefir_linked_stack_push(struct kefir_mem *, struct kefir_list *, void *);
kefir_result_t kefir_linked_stack_pop(struct kefir_mem *, struct kefir_list *, void **);
kefir_result_t kefir_linked_stack_peek(const struct kefir_list *, void **);

#endif
