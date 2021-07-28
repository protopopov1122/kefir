#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_apply_rule_assignment_expression(struct kefir_mem *mem, struct kefir_parser *parser,
                                                             struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, expression, result));
    return KEFIR_OK;
}
