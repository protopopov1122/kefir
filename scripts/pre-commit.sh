#!/usr/bin/env bash

set -e
ROOT="$(git rev-parse --show-toplevel)"

function cleanup {
    make clean
}

cd "$ROOT"
trap cleanup EXIT

make generate -B
make clean
make test MEMCHECK=yes OPT=-O3 SANITIZE=undefined -j$(nproc)
find . \( -name "*.c" -or -name "*.h"  \) -exec clang-format -i -style=file {} \;
