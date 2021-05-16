#ifndef KEFIR_AST_TRANSLATOR_SCOPE_SCOPED_IDENTIFIER_H_
#define KEFIR_AST_TRANSLATOR_SCOPE_SCOPED_IDENTIFIER_H_

#include "kefir/ast/local_context.h"
#include "kefir/ast-translator/environment.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/ir/module.h"
#include "kefir/core/hashtree.h"
#include "kefir/ast/type_layout.h"
#include "kefir/ast-translator/function_declaration.h"

typedef struct kefir_ast_translator_scoped_identifier_entry {
    const char *identifier;
    const struct kefir_ast_scoped_identifier *value;
} kefir_ast_translator_scoped_identifier_t;

typedef struct kefir_ast_translator_scoped_identifier_object {
    kefir_id_t type_id;
    struct kefir_ir_type *type;
    struct kefir_ast_type_layout *layout;
} kefir_ast_translator_scoped_identifier_object_t;

typedef struct kefir_ast_translator_scoped_identifier_function {
    struct kefir_ast_translator_function_declaration *declaration;
} kefir_ast_translator_scoped_identifier_function_t;

_Static_assert(sizeof(struct kefir_ast_translator_scoped_identifier_object) <= KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE,
    "Unable to fit scoped identifier object into payload field");

_Static_assert(sizeof(struct kefir_ast_translator_scoped_identifier_function) <= KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE,
    "Unable to fit scoped identifier function into payload field");

#endif
