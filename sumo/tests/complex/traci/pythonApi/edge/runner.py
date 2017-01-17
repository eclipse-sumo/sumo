#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    runner.py
@author  Michael Behrisch
@author  Daniel Krajzewicz
@date    2011-03-04
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumoBinary,
                                '-c',  'sumo.sumocfg',
                                '--remote-port', str(PORT),
                                '--pedestrian.model', 'nonInteracting'], stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print("step", step)
    traci.simulationStep()
print("edges", traci.edge.getIDList())
print("edge count", traci.edge.getIDCount())
edgeID = "2fi"
print("examining", edgeID)
print("adaptedTraveltime", traci.edge.getAdaptedTraveltime(edgeID, 0))
print("effort", traci.edge.getEffort(edgeID, 0))
print("CO2", traci.edge.getCO2Emission(edgeID))
print("CO", traci.edge.getCOEmission(edgeID))
print("HC", traci.edge.getHCEmission(edgeID))
print("PMx", traci.edge.getPMxEmission(edgeID))
print("NOx", traci.edge.getNOxEmission(edgeID))
print("Fuel", traci.edge.getFuelConsumption(edgeID))
print("Noise", traci.edge.getNoiseEmission(edgeID))
print("Elec", traci.edge.getElectricityConsumption(edgeID))
print("meanSpeed", traci.edge.getLastStepMeanSpeed(edgeID))
print("occupancy", traci.edge.getLastStepOccupancy(edgeID))
print("lastLength", traci.edge.getLastStepLength(edgeID))
print("traveltime", traci.edge.getTraveltime(edgeID))
print("numVeh", traci.edge.getLastStepVehicleNumber(edgeID))
print("haltVeh", traci.edge.getLastStepHaltingNumber(edgeID))
print("vehIds", traci.edge.getLastStepVehicleIDs(edgeID))
print("personIds", traci.edge.getLastStepPersonIDs(edgeID))
print("waiting time", traci.edge.getWaitingTime(edgeID))
# testing unicode
print("checking occupancy with unicode id",
      traci.edge.getLastStepOccupancy(str(edgeID)))

traci.edge.adaptTraveltime(edgeID, 42.)
print("traveltime after adaption", traci.edge.getTraveltime(edgeID))
print("adaptedTraveltime after adaption",
      traci.edge.getAdaptedTraveltime(edgeID, 0))

traci.edge.adaptTraveltime(edgeID, 24., 10, 20)
print("adaptedTraveltime after adaption in interval (check time 0)",
      traci.edge.getAdaptedTraveltime(edgeID, 0))
print("adaptedTraveltime after adaption in interval (check time 15)",
      traci.edge.getAdaptedTraveltime(edgeID, 15))
print("adaptedTraveltime after adaption in interval (check time 25)",
      traci.edge.getAdaptedTraveltime(edgeID, 25))

traci.edge.setEffort(edgeID, 1234.)
print("effort after adaption", traci.edge.getEffort(edgeID, 0))

traci.edge.setEffort(edgeID, 2468., 10, 20)
print("effort after adaption in interval (check time 0)",
      traci.edge.getEffort(edgeID, 0))
print("effort after adaption in interval (check time 15)",
      traci.edge.getEffort(edgeID, 15))
print("effort after adaption in interval (check time 25)",
      traci.edge.getEffort(edgeID, 25))


traci.edge.setMaxSpeed(edgeID, 23.)
print("max speed after adaption", traci.lane.getMaxSpeed(edgeID + "_0"))

traci.edge.subscribe(edgeID)
print(traci.edge.getSubscriptionResults(edgeID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.edge.getSubscriptionResults(edgeID))
for step in range(10):
    traci.simulationStep()
    print("3si count=%s meanSpeed=%s travelTime=%s" % (
        traci.edge.getLastStepVehicleNumber("3si"),
        traci.edge.getLastStepMeanSpeed("3si"),
        traci.edge.getTraveltime("3si")))

traci.close()
sumoProcess.wait()
