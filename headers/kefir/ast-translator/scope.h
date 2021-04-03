#ifndef KEFIR_AST_TRANSLATOR_SCOPE_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_H_

#include "kefir/ast/local_context.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/ir/module.h"
#include "kefir/core/hashtree.h"
#include "kefir/ast/type_layout.h"

typedef struct kefir_ast_translator_scoped_identifier {
    kefir_id_t type_id;
    struct kefir_ir_type *type;
    struct kefir_ast_type_layout *layout;
    kefir_bool_t external;
} kefir_ast_translator_scoped_identifier_t;

_Static_assert(sizeof(struct kefir_ast_translator_scoped_identifier) <= KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE,
    "Unable to fit scoped identifier layout into payload field");

#endif