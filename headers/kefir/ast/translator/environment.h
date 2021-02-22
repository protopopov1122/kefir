#ifndef KEFIR_AST_TRANSLATOR_ENVIRONMENT_H_
#define KEFIR_AST_TRANSLATOR_ENVIRONMENT_H_

#include "kefir/ir/platform.h"
#include "kefir/ast/type.h"

typedef struct kefir_ast_translator_environment {
    struct kefir_ir_target_platform *target_platform;
} kefir_ast_translator_environment_t;

kefir_result_t kefir_ast_translator_environment_init(struct kefir_ast_translator_environment *,
                                                 struct kefir_ir_target_platform *);

kefir_result_t kefir_ast_translator_environment_target_stored_type_info(struct kefir_mem *,
                                                                    const struct kefir_ast_translator_environment *,
                                                                    const struct kefir_ast_type *,
                                                                    struct kefir_ir_target_type_info *);

#endif