#include "kefir/ast-translator/scope/scope_layout_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_scoped_identifer_payload_free(struct kefir_mem *mem,
                                                                  struct kefir_ast_scoped_identifier *scoped_identifier,
                                                                  void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid scoped identifier"));
    if (scoped_identifier->payload.ptr != NULL) {
        switch (scoped_identifier->klass) {
            case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
                                 scoped_identifier->payload.ptr);
                if (scoped_identifier_payload->layout_owner) {
                    REQUIRE_OK(kefir_ast_type_layout_free(mem, scoped_identifier_payload->layout));
                }
                scoped_identifier_payload->type = NULL;
                scoped_identifier_payload->type_id = 0;
                scoped_identifier_payload->layout = NULL;
            } break;

            case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_function *, scoped_identifier_payload,
                                 scoped_identifier->payload.ptr);
                REQUIRE_OK(kefir_ast_translator_function_declaration_free(mem, scoped_identifier_payload->declaration));
                scoped_identifier_payload->declaration = NULL;
            } break;

            case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
            case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
            case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
            case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
                break;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_scoped_identifier_remove(struct kefir_mem *mem, struct kefir_list *list,
                                                             struct kefir_list_entry *entry, void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_id, entry->value);
    scoped_id->identifier = NULL;
    scoped_id->value = NULL;
    KEFIR_FREE(mem, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_scoped_identifier_insert(
    struct kefir_mem *mem, const char *identifier, const struct kefir_ast_scoped_identifier *scoped_identifier,
    struct kefir_list *list) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier"));
    REQUIRE(list != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list"));

    struct kefir_ast_translator_scoped_identifier_entry *scoped_id =
        KEFIR_MALLOC(mem, sizeof(struct kefir_ast_translator_scoped_identifier_entry));
    REQUIRE(scoped_id != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST translator scoped identifier"));
    scoped_id->identifier = identifier;
    scoped_id->value = scoped_identifier;
    kefir_result_t res = kefir_list_insert_after(mem, list, kefir_list_tail(list), scoped_id);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, scoped_id);
        return res;
    });
    return KEFIR_OK;
}
