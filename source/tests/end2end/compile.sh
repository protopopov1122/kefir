#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0
# 
# Copyright (C) 2020-2022  Jevgenijs Protopopovs
# 
# This file is part of Kefir project.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
# # 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

SCRIPT="$0"
BIN_DIR="$1"
SRC_FILE="$2"
DST_FILE="$3"
if [[ "x$DST_FILE" == "x" ]]; then
    echo "Usage: $0 bin_dir source_file destination_file"
    exit -1
fi

KEFIRCC="$BIN_DIR/kefir"
KEFIRFLAGS="--target x86_64-host-none"
TMPDIR="$(mktemp -d)"
INCLUDE_FILE="$(dirname $0)/include.h"
export LD_LIBRARY_PATH="$BIN_DIR/libs"

if [[ "x$PLATFORM" == "xopenbsd" ]]; then
    KEFIRFLAGS="$KEFIRFLAGS -D__OpenBSD__"
fi

function cleanup {
    rm -rf "$TMPDIR"
}

trap cleanup EXIT HUP INT QUIT PIPE TERM
set -e

if [[ -f "$SRC_FILE.profile" ]]; then
    source "$SRC_FILE.profile"
fi

if [[ "x$MEMCHECK" == "xyes" ]]; then
    valgrind $VALGRIND_OPTIONS "$KEFIRCC" $KEFIRFLAGS -I "$(dirname $SRC_FILE)" -I "$(dirname $SCRIPT)/headers" -D KEFIR_END2END_TEST -U __STDC__ -D "KEFIR_END2END=   101   " -W --feature-skip-assembly -W --pp-timestamp=1633204489 \
        -include "$INCLUDE_FILE" "$SRC_FILE" -c -o "$DST_FILE"
else
    "$KEFIRCC" -I "$(dirname $SRC_FILE)"  -I "$(dirname $SCRIPT)/headers" $KEFIRFLAGS -D KEFIR_END2END_TEST -U __STDC__ -D "KEFIR_END2END=   101   " -W --feature-skip-assembly -W --pp-timestamp=1633204489 \
        -include "$INCLUDE_FILE" "$SRC_FILE" -c -o "$DST_FILE"
fi