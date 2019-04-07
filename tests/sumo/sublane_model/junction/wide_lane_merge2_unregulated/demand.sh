#!/bin/bash
$SUMO_HOME/randomTrips.py -n net.net.xml -e 600 -p 0.5 -r input_routes.rou.xml --vehicle-class passenger -t 'departSpeed="max" departPosLat="random"'  
