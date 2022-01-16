#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
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

SUMO_HOME = os.path.join(os.path.dirname(__file__), "..", "..", "..", "..")
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", SUMO_HOME), "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net2.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log",
             ] + sys.argv[1:])

vehID = "bus"

for i in range(500):
    traci.simulationStep()

print("retrieve stops before saving")
print("time", traci.simulation.getTime())
print("  past 10 stops", traci.vehicle.getStops(vehID, -10))
print("  next 10 stops", traci.vehicle.getStops(vehID, 10))

traci.simulation.saveState("state.xml")

traci.load(["-n", "input_net2.net.xml",
            "--load-state", "state.xml",
            "--no-step-log",
            ] + sys.argv[1:])

print("retrieve stops after saving")
for i in range(2):
    print("time", traci.simulation.getTime())
    print("  past 10 stops", traci.vehicle.getStops(vehID, -10))
    print("  next 10 stops", traci.vehicle.getStops(vehID, 10))
    traci.simulationStep()

traci.close()
