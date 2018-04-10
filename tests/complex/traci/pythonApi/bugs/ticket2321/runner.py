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
# @author  Jakob Erdmann
# @date    2017-01-23
# @version $Id$


from __future__ import print_function
from __future__ import absolute_import
import os
import subprocess
import sys
import random
sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
import traci
import sumolib  # noqa

sumoBinary = os.environ["SUMO_BINARY"]
PORT = sumolib.miscutils.getFreeSocketPort()
sumoProcess = subprocess.Popen([sumoBinary,
                                '-c', 'sumo.sumocfg',
                                '-S', '-Q',
                                '--remote-port', str(PORT)], stdout=sys.stdout)


firstPos = None
endPos = None

vehID = "v0"
traci.init(PORT)
traci.simulationStep()
traci.route.add("r0", ["SC", "CN"])
traci.vehicle.add(vehID, "r0")
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    try:
        if firstPos is None:
            firstPos = traci.vehicle.getPosition(vehID)
            endPos = traci.simulation.convert2D("CN", 90)
        currPos = traci.vehicle.getPosition(vehID)
        currEdge = traci.vehicle.getRoadID(vehID)
        currLanePos = traci.vehicle.getLanePosition(vehID)
        print("step=%s road=%s lane=%s pos=%.2f dist=%.2f simDist=%.2f simDistToEnd=%.2f distToInt=%.2f distToEnd=%.2f simDist2DToInt=%.2f simDist2DToEnd=%.2f" % (
            traci.simulation.getCurrentTime() / 1000,
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

    except traci.TraCIException:
        pass

traci.close()
sumoProcess.wait()
