#include "kefir/preprocessor/macro_scope.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t overlay_locate(const struct kefir_preprocessor_macro_scope *scope, const char *identifier,
                                     const struct kefir_preprocessor_macro **macro) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid overlay macro scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor macro"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_overlay_macro_scope *, overlay_scope, scope->payload);

    kefir_result_t res = overlay_scope->overlay->locate(overlay_scope->overlay, identifier, macro);
    if (res == KEFIR_NOT_FOUND) {
        res = overlay_scope->base->locate(overlay_scope->base, identifier, macro);
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_overlay_macro_scope_init(struct kefir_preprocessor_overlay_macro_scope *scope,
                                                           const struct kefir_preprocessor_macro_scope *base,
                                                           const struct kefir_preprocessor_macro_scope *overlay) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to overlay macro scope"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to base macro scope"));
    REQUIRE(overlay != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to overlay macro scope"));

    scope->base = base;
    scope->overlay = overlay;
    scope->scope.payload = scope;
    scope->scope.locate = overlay_locate;
    return KEFIR_OK;
}
