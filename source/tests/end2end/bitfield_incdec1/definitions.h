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

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

struct Structure {
    int a : 6, b : 11, c : 7;
    unsigned d : 10, e : 12;
};

void preinc_a(struct Structure *);
void preinc_b(struct Structure *);
void preinc_c(struct Structure *);
void preinc_d(struct Structure *);
void preinc_e(struct Structure *);

void postinc_a(struct Structure *);
void postinc_b(struct Structure *);
void postinc_c(struct Structure *);
void postinc_d(struct Structure *);
void postinc_e(struct Structure *);

void predec_a(struct Structure *);
void predec_b(struct Structure *);
void predec_c(struct Structure *);
void predec_d(struct Structure *);
void predec_e(struct Structure *);

void postdec_a(struct Structure *);
void postdec_b(struct Structure *);
void postdec_c(struct Structure *);
void postdec_d(struct Structure *);
void postdec_e(struct Structure *);

#endif
