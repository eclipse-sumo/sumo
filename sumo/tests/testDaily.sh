OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME=$PWD
if test x"$1" = x; then
  cd ..
  SUMO_BIN=$PWD/src
else
  SUMO_BIN=$1
fi
cd $OLDDIR
export DFROUTER_BINARY=$SUMO_BIN/sumo-dfrouter
export DUAROUTER_BINARY=$SUMO_BIN/sumo-duarouter
export JTRROUTER_BINARY=$SUMO_BIN/sumo-jtrrouter
export NETCONVERT_BINARY=$SUMO_BIN/sumo-netconvert
export NETGEN_BINARY=$SUMO_BIN/sumo-netgen
export OD2TRIPS_BINARY=$SUMO_BIN/sumo-od2trips
export SUMO_BINARY=$SUMO_BIN/sumo
texttest.py -b
texttest.py -b -coll

