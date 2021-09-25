/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
