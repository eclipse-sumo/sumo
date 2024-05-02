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
# @author  Jakob Erdmann
# @date    2018-09-27

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "--no-step-log",
             ])

print("getDistanceRoad (no connection):",
      traci.simulation.getDistanceRoad("gneE0", 100, "gneE1", 100, isDriving=True))
print("getDistanceRoad (normal to normal):",
      traci.simulation.getDistanceRoad("gneE4", 50, "-gneE2", 50, isDriving=True))
print("getDistanceRoad (normal to 1st internal):",
      traci.simulation.getDistanceRoad("gneE4", 50, ":gneJ5_4", 5, isDriving=True))
print("getDistanceRoad (normal to 2nd internal):",
      traci.simulation.getDistanceRoad("gneE4", 50, ":gneJ5_7", 5, isDriving=True))
print("getDistanceRoad (1st internal to normal):",
      traci.simulation.getDistanceRoad(":gneJ5_4", 5, "-gneE2", 50, isDriving=True))
print("getDistanceRoad (1st internal to 2nd internal):",
      traci.simulation.getDistanceRoad(":gneJ5_4", 5, ":gneJ5_7", 5, isDriving=True))
print("getDistanceRoad (2nd internal to normal):",
      traci.simulation.getDistanceRoad(":gneJ5_7", 5, "-gneE2", 50, isDriving=True))
print("getDistanceRoad (2nd internal to 1st internal):",
      traci.simulation.getDistanceRoad(":gneJ5_7", 5, ":gneJ5_4", 5, isDriving=True))

traci.route.add("r", ["gneE4", "-gneE2"])
traci.vehicletype.setMaxSpeed("DEFAULT_VEHTYPE", 1)
traci.vehicle.add("v", "r")
traci.simulationStep()

print("getDrivingDistance (no connection):",
      traci.vehicle.getDrivingDistance("v", "gneE1", 100))
print("getDrivingDistance (normal to normal):",
      traci.vehicle.getDrivingDistance("v", "-gneE2", 50))
print("getDrivingDistance (normal to 1st internal):",
      traci.vehicle.getDrivingDistance("v", ":gneJ5_4", 5))
print("getDrivingDistance (normal to 2nd internal):",
      traci.vehicle.getDrivingDistance("v", ":gneJ5_7", 5))
while traci.vehicle.getRoadID("v") != ":gneJ5_4":
    traci.simulationStep()
print("getDrivingDistance (1st internal to normal):",
      traci.vehicle.getDrivingDistance("v", "-gneE2", 50))
print("getDrivingDistance (1st internal to 2nd internal):",
      traci.vehicle.getDrivingDistance("v", ":gneJ5_7", 5))
while traci.vehicle.getRoadID("v") != ":gneJ5_7":
    traci.simulationStep()
print("getDrivingDistance (2nd internal to normal):",
      traci.vehicle.getDrivingDistance("v", "-gneE2", 50))
print("getDrivingDistance (2nd internal to 1st internal):",
      traci.vehicle.getDrivingDistance("v", ":gneJ5_4", 5))

traci.close()
