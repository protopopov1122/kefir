#!/usr/bin/env bash

set -e
ROOT="$(git rev-parse --show-toplevel)"

cd "$ROOT" && make generate -B && make test MEMCHECK=yes -j$(nproc)