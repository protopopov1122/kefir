#include "declarator_analysis.h"
#include "kefir/core/util.h"
#include <stdarg.h>

kefir_result_t append_specifiers(struct kefir_mem *mem, struct kefir_ast_declarator_specifier_list *list, int count,
                                 ...) {
    va_list args;
    va_start(args, count);
    while (count--) {
        struct kefir_ast_declarator_specifier *specifier = va_arg(args, struct kefir_ast_declarator_specifier *);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, list, specifier));
    }
    va_end(args);
    return KEFIR_OK;
}
