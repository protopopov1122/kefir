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

#ifndef KEFIR_DRIVER_TARGET_H_
#define KEFIR_DRIVER_TARGET_H_

#include "kefir/core/basic-types.h"

typedef enum kefir_driver_target_arch { KEFIR_DRIVER_TARGET_ARCH_X86_64 } kefir_driver_target_arch_t;

typedef enum kefir_driver_target_platform {
    KEFIR_DRIVER_TARGET_PLATFORM_LINUX,
    KEFIR_DRIVER_TARGET_PLATFORM_FREEBSD,
    KEFIR_DRIVER_TARGET_PLATFORM_OPENBSD,
    KEFIR_DRIVER_TARGET_PLATFORM_NETBSD
} kefir_driver_target_platform_t;

typedef enum kefir_driver_target_variant {
    KEFIR_DRIVER_TARGET_VARIANT_SYSTEM,
    KEFIR_DRIVER_TARGET_VARIANT_GNU,
    KEFIR_DRIVER_TARGET_VARIANT_MUSL,
    KEFIR_DRIVER_TARGET_VARIANT_NONE
} kefir_driver_target_variant_t;

typedef struct kefir_driver_target {
    kefir_driver_target_arch_t arch;
    kefir_driver_target_platform_t platform;
    kefir_driver_target_variant_t variant;
} kefir_driver_target_t;

kefir_result_t kefir_driver_target_match(const char *, struct kefir_driver_target *);
kefir_result_t kefir_driver_target_default(struct kefir_driver_target *);

#endif
