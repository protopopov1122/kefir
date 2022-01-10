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

#ifndef KEFIR_CORE_DATA_MODEL_H_
#define KEFIR_CORE_DATA_MODEL_H_

#include "kefir/core/base.h"

typedef enum kefir_data_model {
    KEFIR_DATA_MODEL_UNKNOWN = 0,
    KEFIR_DATA_MODEL_ILP32,
    KEFIR_DATA_MODEL_LLP64,
    KEFIR_DATA_MODEL_LP64,
    KEFIR_DATA_MODEL_ILP64,
    KEFIR_DATA_MODEL_SILP64
} kefir_data_model_t;

#endif
