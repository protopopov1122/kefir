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

# Compile Lua interpreter with Kefir and run Lua basic test suite

set -e

KEFIRCC="$1"
KEFIRRT="$2"
BUILDDIR="$3"
LUA_VERSION="5.4.3"
SCRIPTDIR="$(realpath $(dirname $0))"

LUA_ARCHIVE="lua-$LUA_VERSION.tar.gz"
LUA_TESTS_ARCHIVE="lua-$LUA_VERSION-tests.tar.gz"
LUA_URL="https://www.lua.org/ftp/lua-$LUA_VERSION.tar.gz"
LUA_TESTS_URL="https://www.lua.org/tests/lua-$LUA_VERSION-tests.tar.gz"
LUA_DIR="lua-$LUA_VERSION"
LUA_TESTS_DIR="lua-$LUA_VERSION-tests"

rm -rf "$BUILDDIR"
mkdir "$BUILDDIR"
pushd "$BUILDDIR"

echo "Downloading Lua..."
wget -O "$LUA_ARCHIVE" "$LUA_URL"
wget -O "$LUA_TESTS_ARCHIVE" "$LUA_TESTS_URL"

echo "Unpacking Lua..."
tar xvf "$LUA_ARCHIVE"
tar xvf "$LUA_TESTS_ARCHIVE"

echo "Building Lua..."
pushd "$LUA_DIR"
mkdir bin
for file in $(find src -maxdepth 1 -mindepth 1 -name "*.c"); do
  echo "        $file..."
  $KEFIRCC -I "$(pwd)/src" -D _GNU_SOURCE "$file" > "$(pwd)/bin/$(basename $file).asm"
done

echo "Assembling Lua..."
find bin -name "*.asm" -exec as -o {}.o {} \;
as "$KEFIRRT" -o bin/runtime.o

echo "Linking Lua..."
musl-gcc -static -no-pie -o bin/lua \
	bin/lapi.c.asm.o \
	bin/lauxlib.c.asm.o \
	bin/lbaselib.c.asm.o \
	bin/lcode.c.asm.o \
	bin/lcorolib.c.asm.o \
	bin/lctype.c.asm.o \
	bin/ldblib.c.asm.o \
	bin/ldebug.c.asm.o \
	bin/ldo.c.asm.o \
	bin/ldump.c.asm.o \
	bin/lfunc.c.asm.o \
	bin/lgc.c.asm.o \
	bin/linit.c.asm.o \
	bin/liolib.c.asm.o \
	bin/llex.c.asm.o \
	bin/lmathlib.c.asm.o \
	bin/lmem.c.asm.o \
	bin/loadlib.c.asm.o \
	bin/lobject.c.asm.o \
	bin/lopcodes.c.asm.o \
	bin/loslib.c.asm.o \
	bin/lparser.c.asm.o \
	bin/lstate.c.asm.o \
	bin/lstring.c.asm.o \
	bin/lstrlib.c.asm.o \
	bin/ltable.c.asm.o \
	bin/ltablib.c.asm.o \
	bin/ltm.c.asm.o \
	bin/lua.c.asm.o \
	bin/lundump.c.asm.o \
	bin/lutf8lib.c.asm.o \
	bin/lvm.c.asm.o \
	bin/lzio.c.asm.o \
	bin/runtime.o

popd

pushd "$LUA_TESTS_DIR"
echo "Patching Lua test suite..."
# Patch is necessary due to musl locale quirks. The same issue arises with GCC-built Lua linked with musl
patch < "$SCRIPTDIR/lua-$LUA_VERSION-tests.patch"

echo "Running Lua basic test suite..."
"../$LUA_DIR/bin/lua" -e"_U=true" all.lua

echo "Successfully executed Lua basic test suite"
