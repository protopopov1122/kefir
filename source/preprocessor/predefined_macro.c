#include "kefir/preprocessor/predefined_macro.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t locate_predefined(const struct kefir_preprocessor_macro_scope *scope, const char *identifier,
                                        const struct kefir_preprocessor_macro **macro) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid overlay macro scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor macro"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_predefined_macro_scope *, predefined_scope, scope->payload);
    UNUSED(predefined_scope);

    return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Requested identifier was not found in predefined macro scope");
}

kefir_result_t kefir_preprocessor_predefined_macro_scope_init(struct kefir_preprocessor_predefined_macro_scope *scope,
                                                              struct kefir_preprocessor *preprocessor) {
    REQUIRE(scope != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to predefined macro scope"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    scope->preprocessor = preprocessor;
    scope->scope.payload = scope;
    scope->scope.locate = locate_predefined;
    return KEFIR_OK;
}
