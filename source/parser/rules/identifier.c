#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_bool_t kefir_parser_match_rule_identifier(const struct kefir_parser *parser) {
    REQUIRE(parser != NULL, false);
    return PARSER_TOKEN_IS_IDENTIFIER(parser, 0);
}

kefir_result_t kefir_parser_reduce_rule_identifier(struct kefir_mem *mem, struct kefir_parser *parser,
                                                   struct kefir_ast_node_base **result) {
    REDUCE_PROLOGUE(mem, parser, result, identifier);
    REQUIRE_ALLOC(
        result,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, parser->symbols, PARSER_LOOKAHEAD(parser, 0)->identifier)),
        "Failed to allocate AST identifier");
    REQUIRE_OK(kefir_parser_shift(mem, parser));
    return KEFIR_OK;
}
