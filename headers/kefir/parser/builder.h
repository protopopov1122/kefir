#ifndef KEFIR_PARSER_BUILDER_H_
#define KEFIR_PARSER_BUILDER_H_

#include "kefir/ast/node.h"
#include "kefir/parser/parser.h"

typedef struct kefir_parser_ast_builder {
    struct kefir_parser *parser;
    struct kefir_list stack;
} kefir_parser_ast_builder_t;

kefir_result_t kefir_parser_ast_builder_init(struct kefir_parser_ast_builder *, struct kefir_parser *);
kefir_result_t kefir_parser_ast_builder_free(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_push(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                             struct kefir_ast_node_base *);
kefir_result_t kefir_parser_ast_builder_pop(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                            struct kefir_ast_node_base **);
kefir_result_t kefir_parser_ast_builder_scan(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                             kefir_parser_rule_fn_t, void *);

typedef kefir_result_t kefir_parser_ast_builder_callback_t(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                           void *);
kefir_result_t kefir_parser_ast_builder_wrap(struct kefir_mem *, struct kefir_parser *, struct kefir_ast_node_base **,
                                             kefir_parser_ast_builder_callback_t, void *);

kefir_result_t kefir_parser_ast_builder_array_subscript(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_function_call(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_function_call_append(struct kefir_mem *, struct kefir_parser_ast_builder *);
kefir_result_t kefir_parser_ast_builder_struct_member(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                      kefir_bool_t, const char *);
kefir_result_t kefir_parser_ast_builder_unary_operation(struct kefir_mem *, struct kefir_parser_ast_builder *,
                                                        kefir_ast_unary_operation_type_t);

#endif
