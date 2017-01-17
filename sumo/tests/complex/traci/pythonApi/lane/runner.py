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
import random
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(3):
    print("step", step)
    traci.simulationStep()
print("lanes", traci.lane.getIDList())
print("lane count", traci.lane.getIDCount())
laneID = "2fi_0"
print("examining", laneID)
print("length", traci.lane.getLength(laneID))
print("maxSpeed", traci.lane.getMaxSpeed(laneID))
print("width", traci.lane.getWidth(laneID))
print("allowed", traci.lane.getAllowed(laneID))
print("disallowed", traci.lane.getDisallowed(laneID))
print("linkNum", traci.lane.getLinkNumber(laneID))
print("links", traci.lane.getLinks(laneID))
print("linksExtended", traci.lane.getLinks(laneID, extended=True))
print("shape", traci.lane.getShape(laneID))
print("edge", traci.lane.getEdgeID(laneID))
print("CO2", traci.lane.getCO2Emission(laneID))
print("CO", traci.lane.getCOEmission(laneID))
print("HC", traci.lane.getHCEmission(laneID))
print("PMx", traci.lane.getPMxEmission(laneID))
print("NOx", traci.lane.getNOxEmission(laneID))
print("Fuel", traci.lane.getFuelConsumption(laneID))
print("Noise", traci.lane.getNoiseEmission(laneID))
print("Elec", traci.lane.getElectricityConsumption(laneID))
print("meanSpeed", traci.lane.getLastStepMeanSpeed(laneID))
print("occupancy", traci.lane.getLastStepOccupancy(laneID))
print("lastLength", traci.lane.getLastStepLength(laneID))
print("traveltime", traci.lane.getTraveltime(laneID))
print("numVeh", traci.lane.getLastStepVehicleNumber(laneID))
print("haltVeh", traci.lane.getLastStepHaltingNumber(laneID))
print("vehIds", traci.lane.getLastStepVehicleIDs(laneID))
print("waiting time", traci.lane.getWaitingTime(laneID))

traci.lane.setAllowed(laneID, ["taxi"])
print("after setAllowed", traci.lane.getAllowed(
    laneID), traci.lane.getDisallowed(laneID))
traci.lane.setDisallowed(laneID, ["bus"])
print("after setDisallowed", traci.lane.getAllowed(
    laneID), traci.lane.getDisallowed(laneID))
traci.lane.setMaxSpeed(laneID, 42.)
print("after setMaxSpeed", traci.lane.getMaxSpeed(laneID))
traci.lane.setLength(laneID, 123.)
print("after setLength", traci.lane.getLength(laneID))

traci.lane.subscribe(laneID)
print(traci.lane.getSubscriptionResults(laneID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.lane.getSubscriptionResults(laneID))
traci.close()
sumoProcess.wait()
