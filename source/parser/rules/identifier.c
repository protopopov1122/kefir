#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_apply_rule_identifier(struct kefir_mem *mem, struct kefir_parser *parser,
                                                  struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0), KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected identifier token"));
    REQUIRE_ALLOC(
        result,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, parser->symbols, PARSER_CURSOR(parser, 0)->identifier)),
        "Failed to allocate AST identifier");
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}
