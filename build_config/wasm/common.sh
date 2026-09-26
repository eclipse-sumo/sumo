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

# @file    common.sh
# @author  Masaya Kataoka
# @date    17.09.2026
#
# Settings shared by build_dependencies.sh and build.sh. Not meant to be
# called directly.

# WASM_EXCEPTIONS is the single knob deciding how exceptions are implemented.
# libsumo throws across the boundary to its dependencies, so SUMO and every
# cross compiled dependency have to agree on it, otherwise a throw never finds
# its catch. Both scripts derive their setting from this one value and
# build.sh refuses to build against a prefix which was built with the other.
WASM_EXCEPTIONS="${WASM_EXCEPTIONS:-wasm}"
case "$WASM_EXCEPTIONS" in
    wasm) WASM_EXCEPTION_FLAG="-fwasm-exceptions" ;;
    legacy) WASM_EXCEPTION_FLAG="-fexceptions" ;;
    *) echo "WASM_EXCEPTIONS has to be 'wasm' or 'legacy', not '$WASM_EXCEPTIONS'" >&2; exit 1 ;;
esac

PREFIX="${1:-$HOME/sumo-wasm-deps}"
# the mode the dependencies in PREFIX were built with
WASM_EXCEPTIONS_STAMP="$PREFIX/lib/sumo-wasm-exceptions"
JOBS="${JOBS:-$(nproc 2> /dev/null || echo 4)}"

if ! command -v emcmake > /dev/null; then
    echo "emcmake not found, please activate the Emscripten SDK first:" >&2
    echo "  source /path/to/emsdk/emsdk_env.sh" >&2
    exit 1
fi
