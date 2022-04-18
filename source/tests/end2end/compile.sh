#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0
# 
# Copyright (C) 2020-2021  Jevgenijs Protopopovs
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

BIN_DIR="$1"
SRC_FILE="$2"
DST_FILE="$3"
if [[ "x$DST_FILE" == "x" ]]; then
    echo "Usage: $0 bin_dir source_file destination_file"
    exit -1
fi

KEFIRCC="$BIN_DIR/kefir"
TMPDIR="$(mktemp -d)"
INCLUDE_FILE="$(dirname $0)/include.h"
export LD_LIBRARY_PATH="$BIN_DIR/libs"

function cleanup {
    rm -rf "$TMPDIR"
}

trap cleanup EXIT HUP INT QUIT PIPE TERM
set -e

if [[ -f "$SRC_FILE.profile" ]]; then
    source "$SRC_FILE.profile"
fi

valgrind $VALGRIND_OPTIONS "$KEFIRCC" -I "$(dirname $SRC_FILE)" -D KEFIR_END2END_TEST --define "KEFIR_END2END=   101   " --pp-timestamp=1633204489 \
    --include "$INCLUDE_FILE" --feature-labels-as-values --feature-statement-expressions --feature-omitted-conditional-operand "$SRC_FILE" > "$TMPDIR/module.asm"
$AS -o "$DST_FILE" "$TMPDIR/module.asm"