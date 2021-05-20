#include "kefir/ast/specifiers.h"

struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_NONE = {
    .inline_function = false,
    .noreturn_function = false
};

struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_NORETURN = {
    .inline_function = false,
    .noreturn_function = true
};

struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_INLINE = {
    .inline_function = true,
    .noreturn_function = false
};

struct kefir_ast_function_specifier KEFIR_AST_FUNCTION_SPECIFIER_INLINE_NORETURN = {
    .inline_function = true,
    .noreturn_function = true
};
