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

#ifndef KEFIR_CORE_PLATFORM_H_
#define KEFIR_CORE_PLATFORM_H_

#include "kefir/core/base.h"

#ifdef __linux__
#define KEFIR_LINUX_HOST_PLATFORM
#elif defined(__FreeBSD__)
#define KEFIR_FREEBSD_HOST_PLATFORM
#elif defined(__OpenBSD__)
#define KEFIR_OPENBSD_HOST_PLATFORM
#elif defined(__NetBSD__)
#define KEFIR_NETBSD_HOST_PLATFORM
#elif defined(__unix__)
#define KEFIR_UNIX_HOST_PLATFORM
#endif

#endif
