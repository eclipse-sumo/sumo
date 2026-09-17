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

# @file    build.sh
# @author  Masaya Kataoka
# @date    17.09.2026
#
# Builds libsumo as a WebAssembly module. The Emscripten SDK has to be
# activated and build_dependencies.sh has to have been run before,
# see build_config/wasm/README.md.
#
# Usage: build.sh [dependency-prefix]

set -euo pipefail

SUMO_SRC="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PREFIX="${1:-$HOME/sumo-wasm-deps}"
BUILD_DIR="${WASM_BUILD_DIR:-$SUMO_SRC/build-wasm}"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

if ! command -v emcmake > /dev/null; then
    echo "emcmake not found, please activate the Emscripten SDK first:" >&2
    echo "  source /path/to/emsdk/emsdk_env.sh" >&2
    exit 1
fi

if [ ! -f "$PREFIX/lib/libxerces-c.a" ]; then
    echo "Xerces-C not found in $PREFIX, please run build_dependencies.sh first." >&2
    exit 1
fi

# ISOLATED_BUILD keeps the WebAssembly artifacts out of the bin directory of a
# possibly co-existing native build.
emcmake cmake -S "$SUMO_SRC" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DISOLATED_BUILD=ON \
    -DCMAKE_FIND_ROOT_PATH="$PREFIX"
cmake --build "$BUILD_DIR" -j "$JOBS" --target libsumojs

echo
echo "Built $(ls "$BUILD_DIR"/bin/libsumo.js "$BUILD_DIR"/bin/libsumo.wasm 2> /dev/null | tr '\n' ' ')"
echo "Try it with: node $SUMO_SRC/build_config/wasm/test_libsumo.js $BUILD_DIR/bin/libsumo.js"
