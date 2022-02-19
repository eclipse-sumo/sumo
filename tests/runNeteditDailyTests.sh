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

if which texttest &> /dev/null; then
  texttest -a netedit.daily.basic "$@" &
  texttest -a netedit.daily.bugs "$@" &
  texttest -a netedit.daily.selection "$@" &
  texttest -a netedit.daily.volatileRecomputing "$@" &
  texttest -a netedit.daily.data.edgeData "$@" &
  texttest -a netedit.daily.data.edgeRelData "$@" &
  texttest -a netedit.daily.additional.access "$@" &
  texttest -a netedit.daily.additional.basic "$@" &
  texttest -a netedit.daily.additional.busstop "$@" &
  texttest -a netedit.daily.additional.calibrator "$@" &
  texttest -a netedit.daily.additional.calibratorflow "$@" &
  texttest -a netedit.daily.additional.chargingstation "$@" &
  texttest -a netedit.daily.additional.containerstop "$@" &
  texttest -a netedit.daily.additional.detentry "$@" &
  texttest -a netedit.daily.additional.detexit "$@" &
  texttest -a netedit.daily.additional.e1detector "$@" &
  texttest -a netedit.daily.additional.e1instantdetector "$@" &
  texttest -a netedit.daily.additional.e2detector "$@" &
  texttest -a netedit.daily.additional.e2multilanedetector "$@" &
  texttest -a netedit.daily.additional.e3detector "$@" &
  texttest -a netedit.daily.additional.lanecalibrator "$@" &
  texttest -a netedit.daily.additional.parkingarea "$@" &
  texttest -a netedit.daily.additional.parkingspace "$@" &
  texttest -a netedit.daily.additional.rerouter "$@" &
  texttest -a netedit.daily.additional.rerouterelements "$@" &
  texttest -a netedit.daily.additional.routeprobe "$@" &
  texttest -a netedit.daily.additional.trainstop "$@" &
  texttest -a netedit.daily.additional.vaporizer "$@" &
  texttest -a netedit.daily.additional.variablespeedsign "$@" &
  texttest -a netedit.daily.shape.basic "$@" &
  texttest -a netedit.daily.shape.poi "$@" &
  texttest -a netedit.daily.shape.poigeo "$@" &
  texttest -a netedit.daily.shape.poilane "$@" &
  texttest -a netedit.daily.shape.poly "$@" &
  texttest -a netedit.daily.TAZ.basic "$@" &
  texttest -a netedit.daily.TAZ.taz "$@"
  wait
  texttest -a netedit.daily.net.connections "$@" &
  texttest -a netedit.daily.net.crossings "$@" &
  texttest -a netedit.daily.net.edges "$@" &
  texttest -a netedit.daily.net.junctions "$@" &
  texttest -a netedit.daily.net.lanes "$@" &
  texttest -a netedit.daily.net.trafficlights "$@" &
  texttest -a netedit.daily.demand.basic "$@" &
  texttest -a netedit.daily.demand.flow "$@" &
  texttest -a netedit.daily.demand.flow_embedded "$@" &
  texttest -a netedit.daily.demand.person "$@" &
  texttest -a netedit.daily.demand.personflow "$@" &
  texttest -a netedit.daily.demand.route "$@" &
  texttest -a netedit.daily.demand.routeflow "$@" &
  texttest -a netedit.daily.demand.trip "$@" &
  texttest -a netedit.daily.demand.vehicle "$@" &
  texttest -a netedit.daily.demand.vehicle_embedded "$@" &
  texttest -a netedit.daily.demand.trip_junctions "$@" &
  texttest -a netedit.daily.demand.vehicletype "$@" &
  texttest -a netedit.daily.demand.stop.basic "$@" &
  texttest -a netedit.daily.demand.stop.busstop "$@" & 
  texttest -a netedit.daily.demand.stop.chargingstation "$@" &
  texttest -a netedit.daily.demand.stop.containerstop "$@" &
  texttest -a netedit.daily.demand.stop.lane "$@" &
  texttest -a netedit.daily.demand.stop.parkingarea "$@" &
  texttest -a netedit.daily.complex "$@"
  wait
else
  texttest.py -a netedit.daily.basic "$@" &
  texttest.py -a netedit.daily.bugs "$@" &
  texttest.py -a netedit.daily.selection "$@" &
  texttest.py -a netedit.daily.volatileRecomputing "$@" &
  texttest.py -a netedit.daily.data.edgeData "$@" &
  texttest.py -a netedit.daily.data.edgeRelData "$@" &
  texttest.py -a netedit.daily.additional.access "$@" &
  texttest.py -a netedit.daily.additional.basic "$@" &
  texttest.py -a netedit.daily.additional.busstop "$@" &
  texttest.py -a netedit.daily.additional.calibrator "$@" &
  texttest.py -a netedit.daily.additional.chargingstation "$@" &
  texttest.py -a netedit.daily.additional.containerstop "$@" &
  texttest.py -a netedit.daily.additional.detentry "$@" &
  texttest.py -a netedit.daily.additional.detexit "$@" &
  texttest.py -a netedit.daily.additional.e1detector "$@" &
  texttest.py -a netedit.daily.additional.e1instantdetector "$@" &
  texttest.py -a netedit.daily.additional.e2detector "$@" &
  texttest.py -a netedit.daily.additional.e2multilanedetector "$@" &
  texttest.py -a netedit.daily.additional.e3detector "$@" &
  texttest.py -a netedit.daily.additional.lanecalibrator "$@" &
  texttest.py -a netedit.daily.additional.parkingarea "$@" &
  texttest.py -a netedit.daily.additional.parkingspace "$@" &
  texttest.py -a netedit.daily.additional.rerouter "$@" &
  texttest.py -a netedit.daily.additional.routeprobe "$@" &
  texttest.py -a netedit.daily.additional.trainstop "$@" &
  texttest.py -a netedit.daily.additional.vaporizer "$@" &
  texttest.py -a netedit.daily.additional.variablespeedsign "$@" &
  texttest.py -a netedit.daily.shape.basic "$@" &
  texttest.py -a netedit.daily.shape.poi "$@" &
  texttest.py -a netedit.daily.shape.poigeo "$@" &
  texttest.py -a netedit.daily.shape.poilane "$@" &
  texttest.py -a netedit.daily.shape.poly "$@" &
  texttest.py -a netedit.daily.TAZ.basic "$@" &
  texttest.py -a netedit.daily.TAZ.taz "$@"
  wait
  texttest.py -a netedit.daily.net.connections "$@" &
  texttest.py -a netedit.daily.net.crossings "$@" &
  texttest.py -a netedit.daily.net.edges "$@" &
  texttest.py -a netedit.daily.net.junctions "$@" &
  texttest.py -a netedit.daily.net.lanes "$@" &
  texttest.py -a netedit.daily.net.trafficlights "$@" &
  texttest.py -a netedit.daily.demand.basic "$@" &
  texttest.py -a netedit.daily.demand.flow "$@" &
  texttest.py -a netedit.daily.demand.flow_embedded "$@" &
  texttest.py -a netedit.daily.demand.person "$@" &
  texttest.py -a netedit.daily.demand.personflow "$@" &
  texttest.py -a netedit.daily.demand.route "$@" &
  texttest.py -a netedit.daily.demand.routeflow "$@" &
  texttest.py -a netedit.daily.demand.trip "$@" &
  texttest.py -a netedit.daily.demand.vehicle "$@" &
  texttest.py -a netedit.daily.demand.vehicle_embedded "$@" &
  texttest.py -a netedit.daily.demand.trip_junctions "$@" &
  texttest.py -a netedit.daily.demand.vehicletype "$@" &
  texttest.py -a netedit.daily.demand.stop.basic "$@" &
  texttest.py -a netedit.daily.demand.stop.busstop "$@" & 
  texttest.py -a netedit.daily.demand.stop.chargingstation "$@" &
  texttest.py -a netedit.daily.demand.stop.containerstop "$@" &
  texttest.py -a netedit.daily.demand.stop.lane "$@" &
  texttest.py -a netedit.daily.demand.stop.parkingarea "$@" &
  texttest.py -a netedit.daily.complex "$@"
  wait
fi
