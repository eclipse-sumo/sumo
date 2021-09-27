netgenerate -c corridor.netgcfg
$SUMO_HOME/tools/randomTrips.py -r background.trips.xml --prefix bg -n net.net.xml --fringe-factor 100 --period 1.8 -t 'departSpeed="max" departLane="best"' --seed 42
duarouter -n net.net.xml -r background.trips.xml,eastwest.trips.xml -o vehroutes.xml
$SUMO_HOME/tools/tlsCycleAdaptation.py -n net.net.xml -r vehroutes.xml -o webster_tls.add.xml -e -p webster
