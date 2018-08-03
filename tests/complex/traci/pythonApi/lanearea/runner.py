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
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))
if len(sys.argv) > 1:
    import libsumo as traci  # noqa
else:
    import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
for step in range(4):
    print("step", step)
    traci.simulationStep()
print("laneareas", traci.lanearea.getIDList())
print("lanearea count", traci.lanearea.getIDCount())

for detID in traci.lanearea.getIDList():
    print("examining", detID)
    print("pos", traci.lanearea.getPosition(detID))
    print("length", traci.lanearea.getLength(detID))
    print("lane", traci.lanearea.getLaneID(detID))
    print("vehNum", traci.lanearea.getLastStepVehicleNumber(detID))
    print("haltNum", traci.lanearea.getLastStepHaltingNumber(detID))
    print("meanSpeed", traci.lanearea.getLastStepMeanSpeed(detID))
    print("vehIDs", traci.lanearea.getLastStepVehicleIDs(detID))
    print("occupancy", traci.lanearea.getLastStepOccupancy(detID))

traci.lanearea.subscribe(detID)
print(traci.lanearea.getSubscriptionResults(detID))
for step in range(3, 6):
    print("step", step)
    traci.simulationStep()
    print(traci.lanearea.getSubscriptionResults(detID))
traci.close()
