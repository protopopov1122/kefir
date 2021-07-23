#include "kefir/ast-translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translate_unit(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                        struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL && node->properties.category == KEFIR_AST_NODE_CATEGORY_TRANSLATION_UNIT &&
                node->klass->type == KEFIR_AST_TRANSLATION_UNIT,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation unit"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));

    ASSIGN_DECL_CAST(struct kefir_ast_translation_unit *, unit, node->self);
    for (const struct kefir_list_entry *iter = kefir_list_head(&unit->external_definitions); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, external_definition, iter->value);

        switch (external_definition->properties.category) {
            case KEFIR_AST_NODE_CATEGORY_DECLARATION:
                // Intentionally left blank
                break;

            case KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION:
                REQUIRE_OK(kefir_ast_translate_function(mem, external_definition, context));
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected external definition node category");
        }
    }
    return KEFIR_OK;
}
