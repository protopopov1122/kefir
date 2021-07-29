#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t KEFIR_PARSER_RULE_FN(assignment_expression)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                           struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, additive_expression, result));
    return KEFIR_OK;
}
