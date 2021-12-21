#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Michael Behrisch
# @author  Daniel Krajzewicz
# @date    2011-03-04


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
for step in range(3):
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
print("position", traci.inductionloop.getPosition(loopID))
print("laneID", traci.inductionloop.getLaneID(loopID))

traci.inductionloop.setParameter(loopID, "foo", "42")
print("parameter", traci.inductionloop.getParameter(loopID, "foo"))
print("parameter from XML", traci.inductionloop.getParameter(loopID, "loadedFromXML"))

traci.inductionloop.subscribe(loopID)
print(traci.inductionloop.getSubscriptionResults(loopID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.inductionloop.getSubscriptionResults(loopID))

for i in range(24):
    print("step=%s occ=%s num=%s detVehs=%s vehData=%s" % (
        traci.simulation.getTime(),
        traci.inductionloop.getLastStepOccupancy(loopID),
        traci.inductionloop.getLastStepVehicleNumber(loopID),
        traci.inductionloop.getLastStepVehicleIDs(loopID),
        traci.inductionloop.getVehicleData(loopID),
    ))
    traci.simulationStep()

traci.close()
