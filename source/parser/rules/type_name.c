#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t KEFIR_PARSER_RULE_FN(type_name)(struct kefir_mem *mem, struct kefir_parser *parser,
                                               struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    // TODO Implement type name parser
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Type name parser is not implemented yet");
}
