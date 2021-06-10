#include "kefir/ast-translator/util.h"
#include "kefir/core/util.h"

const struct kefir_ast_type *kefir_ast_translator_normalize_type(const struct kefir_ast_type *original) {
    REQUIRE(original != NULL, NULL);

    const struct kefir_ast_type *unqualified = kefir_ast_unqualified_type(original);
    if (unqualified->tag == KEFIR_AST_TYPE_ENUMERATION) {
        unqualified = kefir_ast_unqualified_type(unqualified->enumeration_type.underlying_type);
    }
    return unqualified;
}
