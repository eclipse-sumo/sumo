#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-04-12

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net2.net.xml",
             "-r", "input_routes.rou.xml",
             "--device.vehroute.probability", "1",
             "--keep-after-arrival", "5",
             "--no-step-log"] + sys.argv[1:])

traci.simulationStep()
print("on WC:", repr(traci.vehicle.getParameter("v0", "device.vehroute.exitTimes")))

try:
    traci.vehicle.getParameter("v0", "device.vehroute.unknown")
except traci.TraCIException:
    print("caught unsupported vehroute param")

prev_exits = ""
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    if "v0" in traci.vehicle.getIDList() or "v0" in traci.simulation.getArrivedIDList():
        exits = traci.vehicle.getParameter("v0", "device.vehroute.exitTimes")
        if exits != prev_exits:
            print("exitTimes:", exits)
            prev_exits = exits

for _ in range(6):
    traci.simulationStep()

traci.close()
