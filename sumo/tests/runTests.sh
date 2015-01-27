#!/bin/bash
#Bash script for the test run.
#sets environment variables respecting SUMO_BINDIR and starts texttest

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
export SUMO_BINARY="$SUMO_BINDIR/sumo"
export GUISIM_BINARY="$SUMO_BINDIR/sumo-gui"
export MAROUTER_BINARY="$SUMO_BINDIR/marouter"

if which texttest &> /dev/null; then
  texttest "$@"
else
  texttest.py "$@"
fi
