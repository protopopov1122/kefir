#ifndef KEFIR_TEST_UTIL_H_
#define KEFIR_TEST_UTIL_H_

#include "kefir/ast/translator/environment.h"

struct kefir_ir_target_platform *kft_util_get_ir_target_platform();
struct kefir_ast_translator_environment *kft_util_get_translator_environment();

#endif