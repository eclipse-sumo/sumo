#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
# @author  Dong Kemu
# @date    2026-11-04

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib  # noqa
import traci  # noqa

traci.start([sumolib.checkBinary("sumo"),
             "-n", "net.net.xml",
             "-r", "input_routes.rou.xml",
             "--no-step-log", "--begin", "0", "--end", "60",
             "--seed", "1"] + sys.argv[1:])

traci.edge.setAllowed("AB", ["passenger", "delivery"])
traci.edge.setAllowed("BC", ["passenger", "delivery"])
print("internal allowed", " ".join(sorted(traci.lane.getAllowed(":B_0_0"))))

traci.vehicle.add("v", "r", typeID="D", depart=0, departSpeed=1)
arrived = False
for _ in range(60):
    traci.simulationStep()
    if "v" in traci.simulation.getArrivedIDList():
        arrived = True
        break
print("arrived", arrived)
traci.close()
