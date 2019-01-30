#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Jakob Erdmann
# @date    2018-09-27
# @version $Id$

import os
import sys

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))

import traci  # noqa
import sumolib  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "--no-step-log",
             ])

print("getDistanceRoad (no connection):", 
        traci.simulation.getDistanceRoad("gneE0", 100, "gneE1", 100, isDriving="true"))
print("getDistanceRoad (normal to normal):", 
        traci.simulation.getDistanceRoad("gneE4", 50, "-gneE2", 50, isDriving="true"))
print("getDistanceRoad (normal to 1st internal):", 
        traci.simulation.getDistanceRoad("gneE4", 50, ":gneJ5_4", 5, isDriving="true"))
print("getDistanceRoad (normal to 2nd internal):", 
        traci.simulation.getDistanceRoad("gneE4", 50, ":gneJ5_7", 5, isDriving="true"))
print("getDistanceRoad (1st internal to normal):", 
        traci.simulation.getDistanceRoad(":gneJ5_4", 5, "-gneE2", 50, isDriving="true"))
print("getDistanceRoad (1st internal to 2nd internal):", 
        traci.simulation.getDistanceRoad(":gneJ5_4", 5, ":gneJ5_7", 5, isDriving="true"))
print("getDistanceRoad (2nd internal to normal):", 
        traci.simulation.getDistanceRoad(":gneJ5_7", 5, "-gneE2", 50, isDriving="true"))
print("getDistanceRoad (2nd internal to 1st internal):", 
        traci.simulation.getDistanceRoad(":gneJ5_7", 5, ":gneJ5_4", 5, isDriving="true"))

traci.close()
