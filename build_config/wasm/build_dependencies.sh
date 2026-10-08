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

source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

WORKDIR="${WASM_DEPS_WORKDIR:-$PREFIX/src}"
ZLIB_VERSION="${ZLIB_VERSION:-v1.3.1}"
XERCES_VERSION="${XERCES_VERSION:-v3.2.5}"

mkdir -p "$WORKDIR"

# clone, cross compile and install one dependency, any further arguments are
# passed on to cmake
build_dep() {
    local name="$1" url="$2" tag="$3"
    shift 3
    if [ ! -d "$WORKDIR/$name" ]; then
        git clone --depth 1 -b "$tag" "$url" "$WORKDIR/$name"
    fi
    emcmake cmake -S "$WORKDIR/$name" -B "$WORKDIR/$name/build-wasm" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$PREFIX" \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_C_FLAGS="$WASM_EXCEPTION_FLAG" \
        -DCMAKE_CXX_FLAGS="$WASM_EXCEPTION_FLAG" \
        "$@"
    cmake --build "$WORKDIR/$name/build-wasm" -j "$JOBS" --target install
}

# zlib is optional for SUMO but without it no gzipped input or output works
build_dep zlib https://github.com/madler/zlib.git "$ZLIB_VERSION"
# zlib installs stub shared libraries which would be preferred by find_package
rm -f "$PREFIX"/lib/libz.so*

# Xerces-C is the XML parser of SUMO and therefore mandatory. Threads and the
# network accessor are unavailable in the browser sandbox, the iconv transcoder
# is the only one which works with the Emscripten libc.
build_dep xerces-c https://github.com/apache/xerces-c.git "$XERCES_VERSION" \
    -Dnetwork=OFF \
    -Dthreads=OFF \
    -Dtranscoder=gnuiconv \
    -Dmessage-loader=inmemory

# record the mode so that build.sh can refuse a mismatching SUMO build
echo "$WASM_EXCEPTIONS" > "$WASM_EXCEPTIONS_STAMP"

echo
echo "WebAssembly dependencies installed in $PREFIX ($WASM_EXCEPTIONS exceptions)"
