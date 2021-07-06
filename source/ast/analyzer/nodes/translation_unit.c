#include <string.h>
#include "kefir/ast/analyzer/nodes.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_analyze_translation_unit_node(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_translation_unit *node,
                                                       struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST labeled statement"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST base node"));

    REQUIRE_OK(kefir_ast_node_properties_init(&base->properties));
    base->properties.category = KEFIR_AST_NODE_CATEGORY_TRANSLATION_UNIT;

    for (const struct kefir_list_entry *iter = kefir_list_head(&node->external_definitions); iter != NULL;
         kefir_list_next(&iter)) {

        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, entry, iter->value);
        REQUIRE_OK(kefir_ast_analyze_node(mem, context, entry));
        REQUIRE(entry->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION ||
                    entry->properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "AST Translation unit must contain exclusively declarations and function definitions"));
    }

    return KEFIR_OK;
}
