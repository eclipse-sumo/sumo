#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(os.environ['SUMO_HOME'], "tools"))
sys.path.append(os.path.join(
    os.path.dirname(sys.argv[0]), "..", "..", "..", "..", "..", "tools"))
import traci
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')

PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen(
    "%s -c sumo.sumocfg --remote-port %s" % (sumoBinary, PORT), shell=True, stdout=sys.stdout)
traci.init(PORT)
for step in range(4):
    print("step", step)
    traci.simulationStep()
print("inductionloops", traci.inductionloop.getIDList())
print("inductionloop count", traci.inductionloop.getIDCount())
loopID = "0"
print("examining", loopID)
print("vehNum", traci.inductionloop.getLastStepVehicleNumber(loopID))
print("meanSpeed", traci.inductionloop.getLastStepMeanSpeed(loopID))
print("vehIDs", traci.inductionloop.getLastStepVehicleIDs(loopID))
print("occupancy", traci.inductionloop.getLastStepOccupancy(loopID))
print("meanLength", traci.inductionloop.getLastStepMeanLength(loopID))
print("timeSinceDet", traci.inductionloop.getTimeSinceDetection(loopID))
print("vehData", traci.inductionloop.getVehicleData(loopID))

traci.inductionloop.subscribe(loopID)
print(traci.inductionloop.getSubscriptionResults(loopID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.inductionloop.getSubscriptionResults(loopID))

for i in range(24):
    print("step=%s detVehs=%s vehData=%s" % (
        traci.simulation.getCurrentTime() / 1000.0,
        traci.inductionloop.getLastStepVehicleIDs(loopID),
        traci.inductionloop.getVehicleData(loopID),
    ))
    traci.simulationStep()

traci.close()
sumoProcess.wait()
