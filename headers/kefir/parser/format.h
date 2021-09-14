#ifndef KEFIR_PARSER_FORMAT_H_
#define KEFIR_PARSER_FORMAT_H_

#include "kefir/parser/lexem.h"
#include "kefir/parser/lexer.h"
#include "kefir/util/json.h"

kefir_result_t kefir_token_format(struct kefir_json_output *, const struct kefir_token *, kefir_bool_t);
kefir_result_t kefir_token_buffer_format(struct kefir_json_output *, const struct kefir_token_buffer *, kefir_bool_t);

#endif
