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
  texttest -a netedit.daily.net.connections "$@" & 
  texttest -a netedit.daily.net.crossings "$@" & 
  texttest -a netedit.daily.net.edges "$@" & 
  texttest -a netedit.daily.net.junctions "$@" & 
  texttest -a netedit.daily.net.lanes "$@" & 
  texttest -a netedit.daily.net.trafficlights "$@" & 
  texttest -a netedit.daily.additional.access "$@" & 
  texttest -a netedit.daily.additional.basic "$@" & 
  texttest -a netedit.daily.additional.busstop "$@" & 
  texttest -a netedit.daily.additional.calibrator "$@" & 
  texttest -a netedit.daily.additional.chargingstation "$@" & 
  texttest -a netedit.daily.additional.containerstop "$@" & 
  texttest -a netedit.daily.additional.detentry "$@" & 
  texttest -a netedit.daily.additional.detexit "$@" & 
  texttest -a netedit.daily.additional.e1detector "$@" & 
  texttest -a netedit.daily.additional.e1InstantDetector "$@" & 
  texttest -a netedit.daily.additional.e2detector "$@" & 
  texttest -a netedit.daily.additional.e2MultiLaneDetector "$@" & 
  texttest -a netedit.daily.additional.e3detector "$@" & 
  texttest -a netedit.daily.additional.lanecalibrator "$@" & 
  texttest -a netedit.daily.additional.parkingarea "$@" & 
  texttest -a netedit.daily.additional.parkingspace "$@" & 
  texttest -a netedit.daily.additional.rerouter "$@" & 
  texttest -a netedit.daily.additional.routeprobe "$@" & 
  texttest -a netedit.daily.additional.trainstop "$@" & 
  texttest -a netedit.daily.additional.vaporizer "$@" & 
  texttest -a netedit.daily.additional.variablespeedsign "$@" & 
  texttest -a netedit.daily.demand.basic "$@" & 
  texttest -a netedit.daily.demand.flow "$@" & 
  texttest -a netedit.daily.demand.flow_embedded "$@" & 
  texttest -a netedit.daily.demand.person "$@" & 
  texttest -a netedit.daily.demand.personFlow "$@" & 
  texttest -a netedit.daily.demand.route "$@" & 
  texttest -a netedit.daily.demand.routeflow "$@" & 
  texttest -a netedit.daily.demand.stop "$@" & 
  texttest -a netedit.daily.demand.trip "$@" & 
  texttest -a netedit.daily.demand.vehicle "$@" & 
  texttest -a netedit.daily.demand.vehicle_embedded "$@" & 
  texttest -a netedit.daily.demand.vehicletype "$@" & 
  texttest -a netedit.daily.data "$@"
  wait
else
  texttest.py -a netedit.daily.common.basic "$@" & 
  texttest.py -a netedit.daily.common.bugs "$@" & 
  texttest.py -a netedit.daily.common.complex "$@" & 
  texttest.py -a netedit.daily.common.selection "$@" & 
  texttest.py -a netedit.daily.common.volatileRecomputing "$@" & 
  texttest.py -a netedit.daily.net.connections "$@" & 
  texttest.py -a netedit.daily.net.crossings "$@" & 
  texttest.py -a netedit.daily.net.edges "$@" & 
  texttest.py -a netedit.daily.net.junctions "$@" & 
  texttest.py -a netedit.daily.net.lanes "$@" & 
  texttest.py -a netedit.daily.net.trafficlights "$@" & 
  texttest.py -a netedit.daily.additional.access "$@" & 
  texttest.py -a netedit.daily.additional.basic "$@" & 
  texttest.py -a netedit.daily.additional.busstop "$@" & 
  texttest.py -a netedit.daily.additional.calibrator "$@" & 
  texttest.py -a netedit.daily.additional.chargingstation "$@" & 
  texttest.py -a netedit.daily.additional.containerstop "$@" & 
  texttest.py -a netedit.daily.additional.detentry "$@" & 
  texttest.py -a netedit.daily.additional.detexit "$@" & 
  texttest.py -a netedit.daily.additional.e1detector "$@" & 
  texttest.py -a netedit.daily.additional.e1InstantDetector "$@" & 
  texttest.py -a netedit.daily.additional.e2detector "$@" & 
  texttest.py -a netedit.daily.additional.e2MultiLaneDetector "$@" & 
  texttest.py -a netedit.daily.additional.e3detector "$@" & 
  texttest.py -a netedit.daily.additional.lanecalibrator "$@" & 
  texttest.py -a netedit.daily.additional.parkingarea "$@" & 
  texttest.py -a netedit.daily.additional.parkingspace "$@" & 
  texttest.py -a netedit.daily.additional.rerouter "$@" & 
  texttest.py -a netedit.daily.additional.routeprobe "$@" & 
  texttest.py -a netedit.daily.additional.trainstop "$@" & 
  texttest.py -a netedit.daily.additional.vaporizer "$@" & 
  texttest.py -a netedit.daily.additional.variablespeedsign "$@" & 
  texttest.py -a netedit.daily.demand.basic "$@" & 
  texttest.py -a netedit.daily.demand.flow "$@" & 
  texttest.py -a netedit.daily.demand.flow_embedded "$@" & 
  texttest.py -a netedit.daily.demand.person "$@" & 
  texttest.py -a netedit.daily.demand.personFlow "$@" & 
  texttest.py -a netedit.daily.demand.route "$@" & 
  texttest.py -a netedit.daily.demand.routeflow "$@" & 
  texttest.py -a netedit.daily.demand.stop "$@" & 
  texttest.py -a netedit.daily.demand.trip "$@" & 
  texttest.py -a netedit.daily.demand.vehicle "$@" & 
  texttest.py -a netedit.daily.demand.vehicle_embedded "$@" & 
  texttest.py -a netedit.daily.demand.vehicletype "$@" & 
  texttest.py -a netedit.daily.data "$@"
  wait
fi
