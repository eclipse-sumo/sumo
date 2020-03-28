#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2020 German Aerospace Center (DLR) and others.
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


traci.start([sumolib.checkBinary('sumo-gui'),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "-S", "-Q"])

trackedVehIDs = []

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    newVehIDs = [vehID for vehID in traci.lane.getLastStepVehicleIDs('gneE1_0') if vehID not in trackedVehIDs]
    for newVehID in newVehIDs:
        pos = traci.vehicle.getPosition(newVehID)
        shape = [pos, (pos[0]+1, pos[1]), (pos[0]+1, pos[1]+1), (pos[0], pos[1]+1)]
        traci.polygon.add(newVehID, shape, (255, 0, 0), fill=True, layer=100)
        traci.polygon.addDynamics(newVehID, trackedObjectID=newVehID)
        trackedVehIDs.append(newVehID)
    leavingVehIDs = [vehID for vehID in traci.lane.getLastStepVehicleIDs('gneE5_0') if vehID in trackedVehIDs]
    for leavingVehID in leavingVehIDs:
        traci.polygon.remove(leavingVehID)
        trackedVehIDs.remove(leavingVehID)

traci.close()
