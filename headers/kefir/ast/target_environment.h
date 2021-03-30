#ifndef KEFIR_AST_TARGET_ENVIRONMENT_H_
#define KEFIR_AST_TARGET_ENVIRONMENT_H_

#include "kefir/ast/type.h"

typedef struct kefir_ast_target_type_info {
    kefir_size_t size;
    kefir_size_t alignment;
    kefir_bool_t aligned;
} kefir_ast_target_type_info_t;

typedef struct kefir_ast_target_environment {
    kefir_result_t (*type_info)(struct kefir_mem *,
                              const struct kefir_ast_target_environment *,
                              const struct kefir_ast_type *,
                              struct kefir_ast_target_type_info *);
    void *payload;
} kefir_ast_target_environment_t;

#define KEFIR_AST_TARGET_ENVIRONMENT_TYPE_INFO(mem, env, type, info) \
    ((env)->type_info((mem), (env), (type), (info)))

#endif