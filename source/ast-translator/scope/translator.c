#include "kefir/ast-translator/scope/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"


kefir_result_t kefir_ast_translate_global_scope(struct kefir_mem *mem,
                                            struct kefir_ir_module *module,
                                            const struct kefir_ast_translator_global_scope_layout *global_scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(global_scope != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator global scope"));

    for (const struct kefir_list_entry *iter = kefir_list_head(&global_scope->external_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(const struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        if (scoped_identifier->value->object.external) {
            REQUIRE_OK(kefir_ir_module_declare_external(mem, module, scoped_identifier->identifier));
        } else {
            REQUIRE_OK(kefir_ir_module_declare_global(mem, module, scoped_identifier->identifier));
        }
    }
    // TODO Implement thread-local objects
    return KEFIR_OK;
}