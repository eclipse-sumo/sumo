#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
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

OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME="$PWD"
if test x"$SUMO_HOME" = x; then
  cd ..
  export SUMO_HOME="$PWD"
fi
cd $OLDDIR
# for clang sanitizer tests
export LSAN_OPTIONS=suppressions=$SUMO_HOME/build/clang_memleak_suppressions.txt
export UBSAN_OPTIONS=suppressions=$SUMO_HOME/build/clang_ubsan_suppressions.txt

export ACTIVITYGEN_BINARY="$SUMO_HOME/bin/activitygenD"
export DFROUTER_BINARY="$SUMO_HOME/bin/dfrouterD"
export DUAROUTER_BINARY="$SUMO_HOME/bin/duarouterD"
export JTRROUTER_BINARY="$SUMO_HOME/bin/jtrrouterD"
export NETCONVERT_BINARY="$SUMO_HOME/bin/netconvertD"
export NETEDIT_BINARY="$SUMO_HOME/bin/neteditD"
export NETGENERATE_BINARY="$SUMO_HOME/bin/netgenerateD"
export OD2TRIPS_BINARY="$SUMO_HOME/bin/od2tripsD"
export POLYCONVERT_BINARY="$SUMO_HOME/bin/polyconvertD"
export SUMO_BINARY="$SUMO_HOME/bin/sumoD"
export GUISIM_BINARY="$SUMO_HOME/bin/sumo-guiD"
export MAROUTER_BINARY="$SUMO_HOME/bin/marouterD"
export PYTHON="python"

texttest "$@" -v debug
