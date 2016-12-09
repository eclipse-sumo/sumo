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
export NETEDIT_BINARY="$SUMO_BINDIR/netedit"

python $SUMO_HOME/tools/build/runSikulixServer.pyw &
if which texttest &> /dev/null; then
  texttest -a netedit.server "$@"
else
  texttest.py -a netedit.server "$@"
fi

