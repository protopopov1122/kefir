#ifndef KEFIR_AST_SPECIFIERS_H_
#define KEFIR_AST_SPECIFIERS_H_

#include "kefir/ast/base.h"

typedef struct kefir_ast_function_specifier {
    bool inline_function;
    bool noreturn_function;
} kefir_ast_function_specifier_t;

extern struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_NONE;
extern struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_NORETURN;
extern struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_INLINE;
extern struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN;


#endif
