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
# @date    2017-01-23


from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa


def getParams(vehID, index):
    print("stop parameters for index %s at time %s:" % (
        index, traci.simulation.getTime()))

    for p in [
            "index",
            "edge",
            "lane",
            "startPos",
            "endPos",
            "posLat",
            "arrival",
            "duration",
            "until",
            "extension",
            "parking",
            "triggered",
            "permitted",
            "expected",
            "actType",
            "tripId",
            "split",
            "join",
            "line",
            "speed",
            "started",
            "ended",
    ]:
        print(p, traci.vehicle.getStopParameter(vehID, index, p))


sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net4.net.xml",
             "-a", "input_additional4.add.xml",
             "-r", "input_routes.rou.xml",
             "--stop-output", "stopinfos.xml",
             "--no-step-log",
             "--vehroute-output", "vehroutes.xml",
             ] + sys.argv[1:])

vehID = "ego"

while traci.simulation.getMinExpectedNumber() > 0:
    if traci.simulation.getTime() == 5:
        traci.vehicle.setStopParameter(vehID, 0, "lane", "0")
        traci.vehicle.setStopParameter(vehID, 0, "startPos", "5")
        traci.vehicle.setStopParameter(vehID, 0, "endPos", "50")
        traci.vehicle.setStopParameter(vehID, 0, "posLat", "-0.5")
        traci.vehicle.setStopParameter(vehID, 0, "arrival", "42")
        traci.vehicle.setStopParameter(vehID, 0, "duration", "30")
        traci.vehicle.setStopParameter(vehID, 1, "duration", "100")
        traci.vehicle.setStopParameter(vehID, 0, "until", "0:1:40")
        traci.vehicle.setStopParameter(vehID, 0, "extension", "10")
        traci.vehicle.setStopParameter(vehID, 0, "parking", "false")
        traci.vehicle.setStopParameter(vehID, 0, "triggered", "join")
        traci.vehicle.setStopParameter(vehID, 0, "permitted", "p0 p1 p2")
        traci.vehicle.setStopParameter(vehID, 0, "expected", "p0 p1")
        traci.vehicle.setStopParameter(vehID, 0, "actType", "test action")
        traci.vehicle.setStopParameter(vehID, 0, "tripId", "fancyID")
        traci.vehicle.setStopParameter(vehID, 0, "split", "train0")
        traci.vehicle.setStopParameter(vehID, 0, "join", "train1")
        traci.vehicle.setStopParameter(vehID, 0, "line", "S42")
        try:
            traci.vehicle.setStopParameter(vehID, 0, "speed", "3")
        except traci.TraCIException:
            pass
        traci.vehicle.setStopParameter(vehID, 0, "started", "0:1:23")
        traci.vehicle.setStopParameter(vehID, 0, "ended", "0:1:42")
        traci.vehicle.setStopParameter(vehID, 2, "onDemand", "true")
    if traci.simulation.getTime() == 6:
        getParams(vehID, 0)
    if traci.simulation.getTime() == 150:
        getParams(vehID, -1)
    traci.simulationStep()
traci.close()
