#ifndef KEFIR_AST_TARGET_ENVIRONMENT_H_
#define KEFIR_AST_TARGET_ENVIRONMENT_H_

#include "kefir/ast/type.h"
#include "kefir/ast/designator.h"

typedef void *kefir_ast_target_environment_opaque_type_t;

typedef struct kefir_ast_target_type_info {
    kefir_ast_target_environment_opaque_type_t type;

    kefir_size_t size;
    kefir_size_t alignment;
    kefir_bool_t aligned;
} kefir_ast_target_type_info_t;

typedef struct kefir_ast_target_environment {
    kefir_result_t (*type_info)(struct kefir_mem *,
                              const struct kefir_ast_target_environment *,
                              const struct kefir_ast_type *,
                              struct kefir_ast_target_type_info *);
    kefir_result_t (*free_type_info)(struct kefir_mem *,
                                   const struct kefir_ast_target_environment *,
                                   struct kefir_ast_target_type_info *);
    kefir_result_t (*offset_of)(struct kefir_mem *,
                              const struct kefir_ast_target_environment *,
                              kefir_ast_target_environment_opaque_type_t,
                              const struct kefir_ast_designator *,
                              kefir_size_t *);
    void *payload;
} kefir_ast_target_environment_t;

#define KEFIR_AST_TARGET_ENVIRONMENT_TYPE_INFO(mem, env, type, info) \
    ((env)->type_info((mem), (env), (type), (info)))
#define KEFIR_AST_TARGET_ENVIRONMENT_FREE_TYPE_INFO(mem, env, info) \
    ((env)->free_type_info((mem), (env), (info)))
#define KEFIR_AST_TARGET_ENVIRONMENT_OFFSET_OF(mem, env, type, designator, offset_ptr) \
    ((env)->offset_of((mem), (env), (type), (designator), (offset_ptr)))

#endif