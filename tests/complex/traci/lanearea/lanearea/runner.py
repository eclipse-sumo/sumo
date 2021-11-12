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


def check():
    t = traci.simulation.getTime()
    print("step", t)
    if int(t) == t:
        # only report at full seconds to simplify comparison of different step-lengths
        for detID in traci.lanearea.getIDList():
            print("  examining", detID)
            print("     pos", traci.lanearea.getPosition(detID))
            print("     length", traci.lanearea.getLength(detID))
            print("     lane", traci.lanearea.getLaneID(detID))
            print("     vehNum", traci.lanearea.getLastStepVehicleNumber(detID))
            print("     haltNum", traci.lanearea.getLastStepHaltingNumber(detID))
            print("     meanSpeed", traci.lanearea.getLastStepMeanSpeed(detID))
            print("     vehIDs", traci.lanearea.getLastStepVehicleIDs(detID))
            print("     occupancy", traci.lanearea.getLastStepOccupancy(detID))
            print("     jamLengthVeh", traci.lanearea.getJamLengthVehicle(detID))
            print("     jamLengthMet", traci.lanearea.getJamLengthMeters(detID))


traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"] + sys.argv[1:])
check()
print("after the first simulation steps")
while traci.simulation.getTime() < 4:
    traci.simulationStep()
    check()

traci.lanearea.setParameter("det0", "foo", "42")
print("parameter", traci.lanearea.getParameter("det0", "foo"))

print("laneareas", traci.lanearea.getIDList())
print("lanearea count", traci.lanearea.getIDCount())

for detID in traci.lanearea.getIDList():
    traci.lanearea.subscribe(detID)
    print("subscriptionResults: ", traci.lanearea.getSubscriptionResults(detID))
while traci.simulation.getTime() < 10:
    traci.simulationStep()
    check()
    for detID in traci.lanearea.getIDList():
        print("subscriptionResults: ", traci.lanearea.getSubscriptionResults(detID))
traci.simulationStep()
traci.close()
