#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# Bash script for the test run.
# sets environment variables respecting SUMO_BINDIR and starts texttest

OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME="$PWD"
if test x"$SUMO_HOME" = x; then
  cd ..
  export SUMO_HOME="$PWD"
fi
if test x"$SUMO_BINDIR" = x; then
  SUMO_BINDIR="$SUMO_HOME/bin"
fi
cd $OLDDIR
export ACTIVITYGEN_BINARY="$SUMO_BINDIR/activitygen"
export DFROUTER_BINARY="$SUMO_BINDIR/dfrouter"
export DUAROUTER_BINARY="$SUMO_BINDIR/duarouter"
export JTRROUTER_BINARY="$SUMO_BINDIR/jtrrouter"
export NETCONVERT_BINARY="$SUMO_BINDIR/netconvert"
export NETGENERATE_BINARY="$SUMO_BINDIR/netgenerate"
export OD2TRIPS_BINARY="$SUMO_BINDIR/od2trips"
export POLYCONVERT_BINARY="$SUMO_BINDIR/polyconvert"
export SUMO_BINARY="$SUMO_BINDIR/sumo-gui"
export GUISIM_BINARY="$SUMO_BINDIR/sumo-gui"
export MAROUTER_BINARY="$SUMO_BINDIR/marouter"
export PYTHON="python"

if which texttest &> /dev/null; then
  texttest "$@"
else
  texttest.py "$@"
fi
