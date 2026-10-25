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

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary("sumo"),
             "-n", "input_net2.net.xml",
             "-r", "vehicle/tripinfoParameters/input_routes.rou.xml",
             "--device.tripinfo.probability", "1",
             "--no-step-log",
             "--begin", "0"] + sys.argv[1:])

for _ in range(3):
    traci.simulationStep()

for key in ["depart", "departLane", "departPos", "departSpeed",
            "departDelay", "duration", "stopTime", "rerouteNo"]:
    print(key, traci.vehicle.getParameter("v0", "device.tripinfo." + key))

traci.close()
