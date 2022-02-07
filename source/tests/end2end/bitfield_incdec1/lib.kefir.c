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

#include "./definitions.h"

#define PREINC(_field)                          \
    void preinc_##_field(struct Structure *s) { \
        ++s->_field;                            \
    }
#define POSTINC(_field)                          \
    void postinc_##_field(struct Structure *s) { \
        s->_field++;                             \
    }
#define PREDEC(_field)                          \
    void predec_##_field(struct Structure *s) { \
        --s->_field;                            \
    }
#define POSTDEC(_field)                          \
    void postdec_##_field(struct Structure *s) { \
        s->_field--;                             \
    }

#define DEF_FIELD(_field) \
    PREINC(_field)        \
    POSTINC(_field)       \
    PREDEC(_field)        \
    POSTDEC(_field)

DEF_FIELD(a)
DEF_FIELD(b)
DEF_FIELD(c)
DEF_FIELD(d)
DEF_FIELD(e)