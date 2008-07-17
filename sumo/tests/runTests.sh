#!/bin/bash
#Bash script for the test run.
#Accepts two parameter:
#1. Directory for SUMO binaries (defaults to <SUMO_DIST>/src)
#2. Path to texttest.py executable (default is simply texttest.py)

OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME="$PWD"
if test x"$1" = x; then
  cd ..
  SUMO_BIN="$PWD/src"
else
  SUMO_BIN="$1"
fi
cd $OLDDIR
export DFROUTER_BINARY="$SUMO_BIN/sumo-dfrouter"
export DUAROUTER_BINARY="$SUMO_BIN/sumo-duarouter"
export JTRROUTER_BINARY="$SUMO_BIN/sumo-jtrrouter"
export NETCONVERT_BINARY="$SUMO_BIN/sumo-netconvert"
export NETGEN_BINARY="$SUMO_BIN/sumo-netgen"
export OD2TRIPS_BINARY="$SUMO_BIN/sumo-od2trips"
export SUMO_BINARY="$SUMO_BIN/sumo"
export POLYCONVERT_BINARY="$SUMO_BIN/polyconvert"

if test x"$2" = x; then
  TEXTTEST=texttest.py
else
  TEXTTEST="$2"
fi

$TEXTTEST

