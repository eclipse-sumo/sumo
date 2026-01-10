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


sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "--no-step-log",
             "--vehroute-output", "vehroutes.xml",
             "--stop-output", "stopinfos.xml",
             ] + sys.argv[1:])

traci.route.add("r", ["1fi"])
for i in range(3):
    vehID = str(i)
    traci.vehicle.add(vehID, "r")
    traci.vehicle.setStop(vehID, "1fi", 60, duration=400)
    traci.vehicle.setStopParameter(vehID, 0, "startPos", "30")

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
traci.close()
