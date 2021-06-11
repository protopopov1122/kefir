#!/usr/bin/env bash

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
