#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t same_basic_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    return type1->tag == type2->tag;
}

static kefir_result_t free_nothing(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    UNUSED(mem);
    UNUSED(type);
    return KEFIR_OK;
}

static const struct kefir_ast_type SCALAR_VOID = {
    .tag = KEFIR_AST_TYPE_VOID,
    .basic = false,
    .ops = {
        .same = same_basic_type,
        .free = free_nothing
    }
};

const struct kefir_ast_type *kefir_ast_type_void() {
    return &SCALAR_VOID;
}

#define SCALAR_TYPE(id, _tag, _signedness, _rank) \
static const struct kefir_ast_type DEFAULT_SCALAR_##id = { \
    .tag = (_tag), \
    .basic = true, \
    .ops = { \
        .same = same_basic_type, \
        .free = free_nothing \
    }, \
    .basic_type = { \
        .signedness = (_signedness), \
        .rank = (_rank) \
    } \
}; \
\
const struct kefir_ast_type *kefir_ast_type_##id() { \
    return &DEFAULT_SCALAR_##id; \
}

SCALAR_TYPE(bool, KEFIR_AST_TYPE_SCALAR_BOOL, false, 0)
SCALAR_TYPE(unsigned_char, KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR, false, 1)
SCALAR_TYPE(signed_char, KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR, true, 1)
SCALAR_TYPE(unsigned_short, KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT, false, 2)
SCALAR_TYPE(signed_short, KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT, true, 2)
SCALAR_TYPE(unsigned_int, KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT, false, 3)
SCALAR_TYPE(signed_int, KEFIR_AST_TYPE_SCALAR_SIGNED_INT, true, 3)
SCALAR_TYPE(unsigned_long, KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG, false, 4)
SCALAR_TYPE(signed_long, KEFIR_AST_TYPE_SCALAR_SIGNED_LONG, true, 4)
SCALAR_TYPE(unsigned_long_long, KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG, false, 4)
SCALAR_TYPE(signed_long_long, KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG, true, 4)
SCALAR_TYPE(float, KEFIR_AST_TYPE_SCALAR_FLOAT, false, 5)
SCALAR_TYPE(double, KEFIR_AST_TYPE_SCALAR_DOUBLE, false, 6)

#undef SCALAR_TYPE

const struct kefir_ast_type *kefir_ast_type_flip_integer_singedness(const struct kefir_ast_type *type) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_SCALAR_BOOL:
            return type;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
            return kefir_ast_type_signed_char();

        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            return kefir_ast_type_unsigned_char();

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
            return kefir_ast_type_signed_short();

        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            return kefir_ast_type_unsigned_short();

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
            return kefir_ast_type_signed_int();

        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
            return kefir_ast_type_unsigned_int();

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
            return kefir_ast_type_signed_long();

        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
            return kefir_ast_type_unsigned_long();

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
            return kefir_ast_type_signed_long_long();

        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            return kefir_ast_type_unsigned_long_long();
        
        default:
            return NULL;
    }
}