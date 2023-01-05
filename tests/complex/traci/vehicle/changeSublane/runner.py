#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
# @date    2018-09-27

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--lanechange-output", "lanechanges.xml",
             "--lateral-resolution", "0.8",
             "--no-step-log",
             ])

departLane = sys.argv[1]
latDist = float(sys.argv[2])


def reportState(vehID):
    print("t=%s laneIndex=%s posLat=%s state(-1)=%s state(0)=%s state(1)=%s" % (
        traci.simulation.getTime(),
        traci.vehicle.getLaneIndex(vehID),
        traci.vehicle.getLateralLanePosition(vehID),
        traci.vehicle.getLaneChangeStatePretty(vehID, -1),
        traci.vehicle.getLaneChangeStatePretty(vehID,  0),
        traci.vehicle.getLaneChangeStatePretty(vehID,  1)))


vehID = "v0"
traci.vehicle.add(vehID, "r0", departLane=departLane)
traci.vehicle.setLaneChangeMode(vehID, 0)
for i in range(5):
    traci.simulationStep()
traci.vehicle.setParameter(vehID, "lcReason", " sublane_3.2")
traci.vehicle.changeSublane(vehID, latDist)

while traci.simulation.getMinExpectedNumber() > 0:
    reportState(vehID)
    traci.simulationStep()

traci.close()
