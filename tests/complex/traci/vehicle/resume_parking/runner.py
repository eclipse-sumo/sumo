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
# @author  Josh
# @date    2026-11-24

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

# resume a vehicle that waits at a parking stop (the vehicle is not part of a segment queue in meso)
traci.start([sumolib.checkBinary('sumo'), "-n", "input_net.net.xml", "-r", "input_routes.rou.xml",
             "--no-step-log", "--no-warnings"] + sys.argv[1:])
vehID = "Stapler_00"
for i in range(150):
    traci.simulationStep()
print("stops", len(traci.vehicle.getStops(vehID)), traci.vehicle.getRoadID(vehID), traci.vehicle.getSpeed(vehID))
traci.vehicle.resume(vehID)
print("stops after resume", len(traci.vehicle.getStops(vehID)))
for i in range(150):
    traci.simulationStep()
print("arrived", vehID not in traci.vehicle.getIDList())
traci.close()
