#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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

from __future__ import print_function
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net4.net.xml",
             "-r", "input_routes.rou.xml",
             "--stop-output", "stopinfos.xml",
             "--vehroute-output", "vehroutes.xml",
             "--keep-after-arrival", "3",
             "--no-step-log",
             ] + sys.argv[1:])


vehID = "v"
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
for i in range(5):
    traci.simulationStep()
    try:
        print("%s steps after arrival: active=%s loaded=%s" % (
            i,
            traci.vehicle.getIDList(),
            traci.vehicle.getLoadedIDList()))
        print("   vehData: speed=%s pos=%s lane=%s edge=%s stops=%s" % (
            traci.vehicle.getSpeed(vehID),
            traci.vehicle.getPosition(vehID),
            traci.vehicle.getLaneID(vehID),
            traci.vehicle.getRoadID(vehID),
            traci.vehicle.getStops(vehID, -100)
            ))
        print("   vehTripinfo-arrival: time=%s lane=%s pos=%s posLat=%s speed=%s" % (
            traci.vehicle.getParameter(vehID, "device.tripinfo.arrivalTime"),
            traci.vehicle.getParameter(vehID, "device.tripinfo.arrivalLane"),
            traci.vehicle.getParameter(vehID, "device.tripinfo.arrivalPos"),
            traci.vehicle.getParameter(vehID, "device.tripinfo.arrivalPosLat"),
            traci.vehicle.getParameter(vehID, "device.tripinfo.arrivalSpeed"),
            ))
    except traci.TraCIException:
        pass

traci.close()
