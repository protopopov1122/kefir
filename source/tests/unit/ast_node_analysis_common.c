#include "kefir/core/mem.h"
#include "kefir/ast/type.h"
#include "kefir/ast/node.h"

struct kefir_ast_constant *make_constant(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_SCALAR_BOOL:
            return kefir_ast_new_constant_bool(mem, true);

        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            return kefir_ast_new_constant_char(mem, 'A');

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
            return kefir_ast_new_constant_uint(mem, 1234);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
            return kefir_ast_new_constant_int(mem, -1235);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
            return kefir_ast_new_constant_ulong(mem, 100501);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
            return kefir_ast_new_constant_long(mem, -200502);

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
            return kefir_ast_new_constant_ulong_long(mem, 300503);

        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            return kefir_ast_new_constant_long_long(mem, -400504);

        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            return kefir_ast_new_constant_float(mem, 3.14f);

        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            return kefir_ast_new_constant_double(mem, 2.718281828);

        default:
            return NULL;
    }
}
