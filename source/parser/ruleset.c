#include "kefir/parser/ruleset.h"
#include "kefir/parser/rules.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

const struct kefir_parser_ruleset KEFIR_PARSER_DEFAULT_RULESET = {
    .identifier = &KEFIR_PARSER_RULE_IDENTIFIER,
    .constant = &KEFIR_PARSER_RULE_CONSTANT,
    .string_literal = &KEFIR_PARSER_RULE_STRING_LITERAL,
    .generic_selection = &KEFIR_PARSER_RULE_GENERIC_SELECTION,
    .primary_expression = &KEFIR_PARSER_RULE_PRIMARY_EXPRESSION};
