#ifndef KEFIR_PARSER_RULES_H_
#define KEFIR_PARSER_RULES_H_

#include "kefir/parser/parser.h"

#define DECLARE_RULE(_id)                                                                                              \
    kefir_result_t KEFIR_PARSER_RULE_FN(_id)(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **, \
                                             void *)

DECLARE_RULE(identifier);
DECLARE_RULE(constant);
DECLARE_RULE(string_literal);
DECLARE_RULE(generic_selection);
DECLARE_RULE(compound_literal);
DECLARE_RULE(primary_expression);
DECLARE_RULE(postfix_expression);
DECLARE_RULE(unary_expression);
DECLARE_RULE(cast_expression);
DECLARE_RULE(multiplicative_expression);
DECLARE_RULE(additive_expression);
DECLARE_RULE(shift_expression);
DECLARE_RULE(relational_expression);
DECLARE_RULE(equality_expression);
DECLARE_RULE(bitwise_and_expression);
DECLARE_RULE(bitwise_xor_expression);
DECLARE_RULE(bitwise_or_expression);
DECLARE_RULE(logical_and_expression);
DECLARE_RULE(logical_or_expression);
DECLARE_RULE(conditional_expression);
DECLARE_RULE(expression);
DECLARE_RULE(assignment_expression);
DECLARE_RULE(constant_expression);
DECLARE_RULE(type_name);

#undef DECLARE_RULE

kefir_result_t kefir_parser_scan_declaration_specifier(struct kefir_mem *, struct kefir_parser *,
                                                       struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_parser_scan_declaration_specifier_list(struct kefir_mem *, struct kefir_parser *,
                                                            struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_parser_scan_declarator(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_declarator **);

#endif
