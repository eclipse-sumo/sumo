#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    build_dependencies.sh
# @author  Masaya Kataoka
# @date    17.09.2026
#
# Cross compiles the mandatory libsumo dependencies (Xerces-C and zlib) for
# WebAssembly. The Emscripten SDK has to be activated before calling this,
# see build_config/wasm/README.md.
#
# Usage: build_dependencies.sh [install-prefix]

set -euo pipefail

PREFIX="${1:-$HOME/sumo-wasm-deps}"
WORKDIR="${WASM_DEPS_WORKDIR:-$PREFIX/src}"
ZLIB_VERSION="${ZLIB_VERSION:-v1.3.1}"
XERCES_VERSION="${XERCES_VERSION:-v3.2.5}"
# has to match the setting of the SUMO build, see WASM_LEGACY_EXCEPTIONS
EXCEPTION_FLAG="${WASM_EXCEPTION_FLAG:--fwasm-exceptions}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

if ! command -v emcmake > /dev/null; then
    echo "emcmake not found, please activate the Emscripten SDK first:" >&2
    echo "  source /path/to/emsdk/emsdk_env.sh" >&2
    exit 1
fi

mkdir -p "$WORKDIR"

# zlib is optional for SUMO but without it no gzipped input or output works
if [ ! -d "$WORKDIR/zlib" ]; then
    git clone --depth 1 -b "$ZLIB_VERSION" https://github.com/madler/zlib.git "$WORKDIR/zlib"
fi
emcmake cmake -S "$WORKDIR/zlib" -B "$WORKDIR/zlib/build-wasm" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_C_FLAGS="$EXCEPTION_FLAG"
cmake --build "$WORKDIR/zlib/build-wasm" -j "$JOBS" --target install
# zlib installs stub shared libraries which would be preferred by find_package
rm -f "$PREFIX"/lib/libz.so*

# Xerces-C is the XML parser of SUMO and therefore mandatory. Threads and the
# network accessor are unavailable in the browser sandbox, the iconv transcoder
# is the only one which works with the Emscripten libc.
if [ ! -d "$WORKDIR/xerces-c" ]; then
    git clone --depth 1 -b "$XERCES_VERSION" https://github.com/apache/xerces-c.git "$WORKDIR/xerces-c"
fi
emcmake cmake -S "$WORKDIR/xerces-c" -B "$WORKDIR/xerces-c/build-wasm" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DBUILD_SHARED_LIBS=OFF \
    -Dnetwork=OFF \
    -Dthreads=OFF \
    -Dtranscoder=gnuiconv \
    -Dmessage-loader=inmemory \
    -DCMAKE_C_FLAGS="$EXCEPTION_FLAG" \
    -DCMAKE_CXX_FLAGS="$EXCEPTION_FLAG"
cmake --build "$WORKDIR/xerces-c/build-wasm" -j "$JOBS" --target install

echo
echo "WebAssembly dependencies installed in $PREFIX"
