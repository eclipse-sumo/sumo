#!/bin/bash
OUT_DIR=/root/mnt/`date +%Y-%m-%d-%H-%M-%S`
SUMO_ROOT=/root/gRPC/sumo/
SCRIPTS_ROOT=$SUMO_ROOT/build/docker/
CONF_ROOT=$SUMO_ROOT/tests/complex/hybrid_sumo_jps
cd
mkdir $OUT_DIR
echo "running hybrid simulation"
if [ -f "/usr/local/bin/jpscore" ]; then
	jpscore --as-a-service -p 9000 --dump-scenario $OUT_DIR &
else 
	echo "jpscore is not installed; going to build an install the latest version"
	$SCRIPTS_ROOT/update_jps.sh
	jpscore --as-a-service -p 9000 --dump-scenario $OUT_DIR &
fi
if [ -f "/usr/local/bin/sumo" ]; then
	cd
	sumo -c $CONF_ROOT/sumo.sumocfg 
	#--fcd-output $OUT_DIR/fcd.dump
else
	echo "sumo is not installed; going to build an install the latest version"
	$SCRIPTS_ROOT/update_sumo.sh
	cd
	sumo -c $CONF_ROOT/sumo.sumocfg 
	#--fcd-output $OUT_DIR/fcd.dump
fi


sleep 5
