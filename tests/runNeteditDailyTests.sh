#!/bin/bash
#Bash script for the daily netedit test run.
#sets environment variables respecting SUMO_HOME and starts texttest for all existing netedit daily suites

if test `uname` = "Darwin"; then #macOS specific exports
  export LC_ALL=C
  export LANG=C
fi

pushd `dirname $0`
export TEXTTEST_HOME="$PWD"
if test x"$SUMO_HOME" = x; then
  cd ..
  export SUMO_HOME="$PWD"
fi
popd
export NETEDIT_BINARY="$SUMO_HOME/bin/netedit"

for f in $TEXTTEST_HOME/netedit/testsuite.netedit.daily.*; do
  base=`basename $f`
  texttest -a ${base:10} "$@" &
done
wait
