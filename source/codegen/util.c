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

#include "kefir/codegen/util.h"

kefir_size_t kefir_codegen_pad_aligned(kefir_size_t offset, kefir_size_t alignment) {
    if (alignment == 0) {
        return offset;
    }
    const kefir_size_t padding = offset % alignment;
    if (padding != 0) {
        offset += alignment - padding;
    }
    return offset;
}
