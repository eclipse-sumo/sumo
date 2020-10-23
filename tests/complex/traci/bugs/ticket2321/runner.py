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
# @author  Jakob Erdmann
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'), "-c", "sumo.sumocfg"])
firstPos = None
endPos = None

vehID = "v0"
traci.simulationStep()
traci.route.add("r0", ["SC", "CN"])
traci.vehicle.addLegacy(vehID, "r0")
traci.vehicle.setImperfection(vehID, 0)
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    try:
        if firstPos is None:
            firstPos = traci.vehicle.getPosition(vehID)
            endPos = traci.simulation.convert2D("CN", 90)
        currPos = traci.vehicle.getPosition(vehID)
        currEdge = traci.vehicle.getRoadID(vehID)
        currLanePos = traci.vehicle.getLanePosition(vehID)
        print(("step=%s road=%s lane=%s pos=%.2f dist=%.2f simDist=%.2f simDistToEnd=%.2f distToInt=%.2f " +
               "distToEnd=%.2f simDist2DToInt=%.2f simDist2DToEnd=%.2f") % (
            traci.simulation.getTime(),
            currEdge,
            traci.vehicle.getLaneID(vehID),
            traci.vehicle.getLanePosition(vehID),
            traci.vehicle.getDistance(vehID),
            traci.simulation.getDistance2D(firstPos[0], firstPos[1], currPos[0], currPos[1], isDriving=True),
            traci.simulation.getDistance2D(currPos[0], currPos[1], endPos[0], endPos[1], isDriving=True),
            traci.vehicle.getDrivingDistance(vehID, ":C_10", 13.8),
            traci.vehicle.getDrivingDistance(vehID, "CN", 90),
            traci.simulation.getDistanceRoad(currEdge, currLanePos, ":C_10", 13.8),
            traci.simulation.getDistanceRoad(currEdge, currLanePos, "CN", 90)
        ))
    except traci.TraCIException as e:
        if traci.isLibsumo():
            print(e, file=sys.stderr)

traci.close()
