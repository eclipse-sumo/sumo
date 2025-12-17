#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
#
# Bash script for the test run of the debug version.
# Sets environment variables respecting SUMO_HOME and starts texttest.

export LC_ALL=C
export LANG=C

if [[ $(uname) == MINGW* || $(uname) == CYGWIN* ]]; then
  suffix = ".exe"
fi
pushd `dirname $0` > /dev/null
export TEXTTEST_HOME="$PWD"
if test x"$SUMO_HOME" = x; then
  cd ..
  export SUMO_HOME="$PWD"
fi
popd > /dev/null

# for clang sanitizer tests
export LSAN_OPTIONS=suppressions="$SUMO_HOME/build_config/clang_memleak_suppressions.txt,print_suppressions=0"
export UBSAN_OPTIONS=suppressions="$SUMO_HOME/build_config/clang_ubsan_suppressions.txt"

export ACTIVITYGEN_BINARY="$SUMO_HOME/bin/activitygenD$suffix"
export DFROUTER_BINARY="$SUMO_HOME/bin/dfrouterD$suffix"
export DUAROUTER_BINARY="$SUMO_HOME/bin/duarouterD$suffix"
export JTRROUTER_BINARY="$SUMO_HOME/bin/jtrrouterD$suffix"
export NETCONVERT_BINARY="$SUMO_HOME/bin/netconvertD$suffix"
export NETEDIT_BINARY="$SUMO_HOME/bin/neteditD$suffix"
export NETGENERATE_BINARY="$SUMO_HOME/bin/netgenerateD$suffix"
export OD2TRIPS_BINARY="$SUMO_HOME/bin/od2tripsD$suffix"
export POLYCONVERT_BINARY="$SUMO_HOME/bin/polyconvertD$suffix"
export SUMO_BINARY="$SUMO_HOME/bin/sumoD$suffix"
export GUISIM_BINARY="$SUMO_HOME/bin/sumo-guiD$suffix"
export MAROUTER_BINARY="$SUMO_HOME/bin/marouterD$suffix"
export PYTHON="python"

texttest "$@" -v debug
