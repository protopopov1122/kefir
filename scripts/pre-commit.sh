#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0
# 
# Copyright (C) 2020-2021  Jevgenijs Protopopovs
# 
# This file is part of Sloked project.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

set -e
ROOT="$(git rev-parse --show-toplevel)"

function cleanup {
    make clean
}
function format_file {
	file="$1"
	if [[ -f "$file" ]]; then
		clang-format --style=file -i "$file"
		git add "$file"
	fi
}


cd "$ROOT"
trap cleanup EXIT

make generate -B
make clean
make test MEMCHECK=yes OPT=-O3 SANITIZE=undefined -j$(nproc)
for file in `git diff-index --cached --name-only HEAD | grep '\.c$\|\.h$'` ; do
    format_file "$file"
done
