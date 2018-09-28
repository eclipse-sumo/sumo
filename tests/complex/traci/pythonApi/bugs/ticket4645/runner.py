#!/usr/bin/env python
import os
import sys
sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib
import traci


traci.start([sumolib.checkBinary('sumo'), "-c", "data/k0001.sumocfg"])
states = open("data/state.txt").readlines()
step = 0
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    traci.trafficlight.setRedYellowGreenState("K0001", states[step])
    step += 1
