#include "kefir/core/linked_stack.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_linked_stack_push(struct kefir_mem *mem, struct kefir_list *list, void *value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list"));
    REQUIRE_OK(kefir_list_insert_after(mem, list, kefir_list_tail(list), value));
    return KEFIR_OK;
}

kefir_result_t kefir_linked_stack_pop(struct kefir_mem *mem, struct kefir_list *list, void **value) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list"));
    if (value != NULL) {
        *value = NULL;
    }
    struct kefir_list_entry *tail = kefir_list_tail(list);
    if (tail != NULL) {
        if (value != NULL) {
            *value = tail->value;
        }
        REQUIRE_OK(kefir_list_pop(mem, list, tail));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_linked_stack_peek(const struct kefir_list *list, void **value) {
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid value pointer"));
    *value = NULL;
    struct kefir_list_entry *tail = kefir_list_tail(list);
    if (tail != NULL) {
        *value = tail->value;
    }
    return KEFIR_OK;
}
