#ifndef KEFIR_AST_TRANSLATOR_ENVIRONMENT_H_
#define KEFIR_AST_TRANSLATOR_ENVIRONMENT_H_

#include "kefir/ir/platform.h"
#include "kefir/ast/type.h"
#include "kefir/ast/type_layout.h"
#include "kefir/ast/target_environment.h"

typedef struct kefir_ast_translator_environment_type {
    const struct kefir_ast_type *ast_type;
    struct kefir_ir_type type;
    struct kefir_ast_type_layout *layout;
    kefir_ir_target_platform_opaque_type_t target_type;
} kefir_ast_translator_environment_type_t;

typedef struct kefir_ast_translator_environment {
    struct kefir_ast_target_environment target_env;
    struct kefir_ir_target_platform *target_platform;
} kefir_ast_translator_environment_t;

kefir_result_t kefir_ast_translator_environment_init(struct kefir_ast_translator_environment *,
                                                 struct kefir_ir_target_platform *);

#endif
