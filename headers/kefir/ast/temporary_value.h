#ifndef KEFIR_AST_TEMPORARY_VALUE_H_
#define KEFIR_AST_TEMPORARY_VALUE_H_

#include "kefir/core/basic-types.h"

typedef struct kefir_ast_temporary_value_identifier {
    kefir_id_t temporary_id;
    kefir_id_t temporary_field_id;
} kefir_ast_temporary_value_identifier_t;

#endif