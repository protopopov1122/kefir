/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

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

#include "kefir/driver/target.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"
#include "kefir/core/platform.h"
#include <string.h>

static kefir_result_t match_arch(const char *spec, struct kefir_driver_target *target, const char **next) {
    const char *delim = strchr(spec, '-');
    REQUIRE(delim != NULL, KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Architecture specification is not found"));
    if (strncmp("x86_64", spec, delim - spec) == 0) {
        target->arch = KEFIR_DRIVER_TARGET_ARCH_X86_64;
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Architecture specification is not found");
    }
    *next = delim + 1;
    return KEFIR_OK;
}

static kefir_result_t select_host_platform(struct kefir_driver_target *target) {
#ifdef KEFIR_LINUX_HOST_PLATFORM
    target->platform = KEFIR_DRIVER_TARGET_PLATFORM_LINUX;
#elif defined(KEFIR_FREEBSD_HOST_PLATFORM)
    target->platform = KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD;
#elif defined(KEFIR_OPENBSD_HOST_PLATFORM)
    target->platform = KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD;
#elif defined(KEFIR_NETBSD_HOST_PLATFORM)
    target->platform = KEFIR_DRIVER_TARGET_PLATFORM_NETBSD;
#endif
    return KEFIR_OK;
}

static kefir_result_t match_platform(const char *spec, struct kefir_driver_target *target, const char **next) {
    const char *delim = strchr(spec, '-');
    REQUIRE(delim != NULL, KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Platform specification is not found"));
    if (strncmp("linux", spec, delim - spec) == 0) {
        target->platform = KEFIR_DRIVER_TARGET_PLATFORM_LINUX;
    } else if (strncmp("freebsd", spec, delim - spec) == 0) {
        target->platform = KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD;
    } else if (strncmp("openbsd", spec, delim - spec) == 0) {
        target->platform = KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD;
    } else if (strncmp("netbsd", spec, delim - spec) == 0) {
        target->platform = KEFIR_DRIVER_TARGET_PLATFORM_NETBSD;
    } else if (strncmp("host", spec, delim - spec) == 0) {
        REQUIRE_OK(select_host_platform(target));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Platform specification is not found");
    }
    *next = delim + 1;
    return KEFIR_OK;
}

static kefir_result_t select_default_variant(struct kefir_driver_target *target) {
    target->variant = KEFIR_DRIVER_TARGET_VARIANT_NONE;
    if (target->arch == KEFIR_DRIVER_TARGET_ARCH_X86_64 && target->platform == KEFIR_DRIVER_TARGET_PLATFORM_LINUX) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_GNU;
    } else if (target->arch == KEFIR_DRIVER_TARGET_ARCH_X86_64 &&
               target->platform == KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_SYSTEM;
    } else if (target->arch == KEFIR_DRIVER_TARGET_ARCH_X86_64 &&
               target->platform == KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_SYSTEM;
    } else if (target->arch == KEFIR_DRIVER_TARGET_ARCH_X86_64 &&
               target->platform == KEFIR_DRIVER_TARGET_PLATFORM_NETBSD) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_SYSTEM;
    }
    return KEFIR_OK;
}

static kefir_result_t match_variant(const char *spec, struct kefir_driver_target *target) {
    if (strcmp("none", spec) == 0) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_NONE;
    } else if (strcmp("musl", spec) == 0) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_MUSL;
    } else if (strcmp("gnu", spec) == 0) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_GNU;
    } else if (strcmp("system", spec) == 0) {
        target->variant = KEFIR_DRIVER_TARGET_VARIANT_SYSTEM;
    } else if (strcmp("default", spec) == 0) {
        REQUIRE_OK(select_default_variant(target));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_FOUND, "Variant specification is not found");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_driver_target_match(const char *spec, struct kefir_driver_target *target) {
    REQUIRE(spec != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid target specification"));
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to driver target"));

    REQUIRE_OK(match_arch(spec, target, &spec));
    REQUIRE_OK(match_platform(spec, target, &spec));
    REQUIRE_OK(match_variant(spec, target));
    return KEFIR_OK;
}

kefir_result_t kefir_driver_target_default(struct kefir_driver_target *target) {
    REQUIRE(target != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to driver target"));

    target->arch = KEFIR_DRIVER_TARGET_ARCH_X86_64;
    REQUIRE_OK(select_host_platform(target));
    REQUIRE_OK(select_default_variant(target));
    return KEFIR_OK;
}
