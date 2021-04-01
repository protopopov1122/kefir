#ifndef KEFIR_AST_OBJECT_H_
#define KEFIR_AST_OBJECT_H_

#include "kefir/ast/base.h"

typedef struct kefir_ast_target_environment_object_info {
    kefir_size_t size;
    kefir_size_t alignment;
    kefir_bool_t aligned;
    kefir_size_t relative_offset;
} kefir_ast_target_environment_object_info_t;

#endif