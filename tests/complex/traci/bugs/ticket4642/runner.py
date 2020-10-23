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
# @author  Mirko Barthauer (Technische Universitaet Braunschweig)
# @date    2018-09-27

import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml", "-r",
             "input_routes.rou.xml",
             "--no-step-log",
             ])

vehHasLeft = False
vehHasArrived = False
vehID = 'v1'
route = []
step = 0
resultList = [("step", "currentEdge", "targetEdge", "distoToTargetEdge")]

while not vehHasLeft:
    if not vehHasArrived:
        arrivedVehicles = traci.simulation.getDepartedIDList()
        vehHasArrived = vehID in arrivedVehicles
        route = traci.vehicle.getRoute(vehID)

    if vehHasArrived:
        remainingDist = traci.vehicle.getDrivingDistance(vehID, route[-1], 0.0)
        currentEdge = traci.vehicle.getRoadID(vehID)
        vehHasLeft = currentEdge == route[-1]

        if not vehHasLeft:
            resultList.append((str(step), currentEdge, route[-1], str(remainingDist)))
    step += 1
    traci.simulationStep()
traci.close()

# print distance to target edge of the vehicle's route
for line in resultList:
    print("%s" % ";".join(line))
