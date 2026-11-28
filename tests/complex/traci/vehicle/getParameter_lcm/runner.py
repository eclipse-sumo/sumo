#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
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
# @author  Mark Colley
# @date    2026-11-26

from __future__ import print_function
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
             "--no-step-log",
             ] + sys.argv[1:])

# lane change model parameters are defined in the vType (lcStrategic) or not defined (lcCooperative)
for vehID in ("v0",):
    for key in ("lcStrategic", "lcCooperative"):
        try:
            print(vehID, key, float(traci.vehicle.getParameter(vehID, "laneChangeModel." + key)))
        except traci.TraCIException:
            print(vehID, key, "failed")
    try:
        traci.vehicle.setParameter(vehID, "laneChangeModel.lcStrategic", "3")
        print(vehID, "set", float(traci.vehicle.getParameter(vehID, "laneChangeModel.lcStrategic")))
    except traci.TraCIException:
        print(vehID, "set failed")
traci.simulationStep()
traci.close()
