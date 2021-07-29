#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t KEFIR_PARSER_RULE_FN(string_literal)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                    struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE(PARSER_TOKEN_IS_STRING_LITERAL(parser, 0),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected string literal token"));
    const struct kefir_token *token = PARSER_CURSOR(parser, 0);
    REQUIRE_ALLOC(result,
                  KEFIR_AST_NODE_BASE(
                      kefir_ast_new_string_literal(mem, token->string_literal.content, token->string_literal.length)),
                  "Failed to allocate AST string literal");
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}
