#!/bin/bash
#Bash script for the test run.
#sets environment variables respecting SUMO_HOME and starts texttest with python3 for the tool tests

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
export ACTIVITYGEN_BINARY="$SUMO_HOME/bin/activitygen"
export DFROUTER_BINARY="$SUMO_HOME/bin/dfrouter"
export DUAROUTER_BINARY="$SUMO_HOME/bin/duarouter"
export JTRROUTER_BINARY="$SUMO_HOME/bin/jtrrouter"
export NETCONVERT_BINARY="$SUMO_HOME/bin/netconvert"
export NETEDIT_BINARY="$SUMO_HOME/bin/netedit"
export NETGENERATE_BINARY="$SUMO_HOME/bin/netgenerate"
export OD2TRIPS_BINARY="$SUMO_HOME/bin/od2trips"
export POLYCONVERT_BINARY="$SUMO_HOME/bin/polyconvert"
export SUMO_BINARY="$SUMO_HOME/bin/sumo"
export GUISIM_BINARY="$SUMO_HOME/bin/sumo-gui"
export MAROUTER_BINARY="$SUMO_HOME/bin/marouter"
export PYTHON="python3"

texttest -a tools "$@"
