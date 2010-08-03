#!/bin/bash
#Bash script for the test run.
#Accepts two parameter:
#1. Directory for SUMO binaries (defaults to <SUMO_DIST>/bin)
#2. Path to texttest.py executable (default is simply texttest.py)

OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME="$PWD"
if test x"$1" = x; then
  cd ..
  SUMO_BIN="$PWD/bin"
else
  SUMO_BIN="$1"
fi
cd $OLDDIR
export ACTIVITYGEN_BINARY="$SUMO_BIN/activitygen"
export DFROUTER_BINARY="$SUMO_BIN/dfrouter"
export DUAROUTER_BINARY="$SUMO_BIN/duarouter"
export JTRROUTER_BINARY="$SUMO_BIN/jtrrouter"
export NETCONVERT_BINARY="$SUMO_BIN/netconvert"
export NETGEN_BINARY="$SUMO_BIN/netgen"
export OD2TRIPS_BINARY="$SUMO_BIN/od2trips"
export POLYCONVERT_BINARY="$SUMO_BIN/polyconvert"
export SUMO_BINARY="$SUMO_BIN/sumo"
export GUISIM_BINARY="$SUMO_BIN/sumo-gui"

if test x"$2" = x; then
  TEXTTEST=texttest.py
else
  TEXTTEST="$2"
fi

$TEXTTEST

