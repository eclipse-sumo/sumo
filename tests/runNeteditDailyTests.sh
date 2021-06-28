#!/bin/bash
#Bash script for the test run.
#sets environment variables respecting SUMO_BINDIR and starts texttest

if test `uname` = "Darwin"; then #macOS specific exports
  export LC_ALL=C
  export LANG=C
fi

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
export NETEDIT_BINARY="$SUMO_BINDIR/netedit"
export NETGENERATE_BINARY="$SUMO_BINDIR/netgenerate"
export OD2TRIPS_BINARY="$SUMO_BINDIR/od2trips"
export POLYCONVERT_BINARY="$SUMO_BINDIR/polyconvert"
export SUMO_BINARY="$SUMO_BINDIR/sumo"
export GUISIM_BINARY="$SUMO_BINDIR/sumo-gui"
export MAROUTER_BINARY="$SUMO_BINDIR/marouter"
export PYTHON="python"

if which texttest &> /dev/null; then
  texttest -a netedit.daily.common.basic "$@" & 
  texttest -a netedit.daily.common.bugs "$@" & 
  texttest -a netedit.daily.common.complex "$@" & 
  texttest -a netedit.daily.common.selection "$@" & 
  texttest -a netedit.daily.common.volatileRecomputing "$@" & 
  texttest -a netedit.daily.net1 "$@" & 
  texttest -a netedit.daily.net2 "$@" & 
  texttest -a netedit.daily.additional1 "$@" & 
  texttest -a netedit.daily.additional2 "$@" & 
  texttest -a netedit.daily.additional3 "$@" & 
  texttest -a netedit.daily.additional4 "$@" & 
  texttest -a netedit.daily.demand1 "$@" & 
  texttest -a netedit.daily.demand2 "$@" & 
  texttest -a netedit.daily.demand3 "$@" & 
  texttest -a netedit.daily.demand4 "$@" & 
  texttest -a netedit.daily.data "$@"
  wait
else
  texttest.py -a netedit.daily.common.basic "$@" & 
  texttest.py -a netedit.daily.common.bugs "$@" & 
  texttest.py -a netedit.daily.common.complex "$@" & 
  texttest.py -a netedit.daily.common.selection "$@" & 
  texttest.py -a netedit.daily.common.volatileRecomputing "$@" & 
  texttest.py -a netedit.daily.net1 "$@" & 
  texttest.py -a netedit.daily.net2 "$@" & 
  texttest.py -a netedit.daily.additional1 "$@" & 
  texttest.py -a netedit.daily.additional2 "$@" & 
  texttest.py -a netedit.daily.additional3 "$@" & 
  texttest.py -a netedit.daily.additional4 "$@" & 
  texttest.py -a netedit.daily.demand1 "$@" & 
  texttest.py -a netedit.daily.demand2 "$@" & 
  texttest.py -a netedit.daily.demand3 "$@" & 
  texttest.py -a netedit.daily.demand4 "$@" & 
  texttest.py -a netedit.daily.data "$@"
  wait
fi
