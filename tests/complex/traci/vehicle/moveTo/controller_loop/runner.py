#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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

WATCH = False
TS = 1.0

sumoBinary = sumolib.checkBinary("sumo")
cmd = [
    sumoBinary,
    '-n', 'input_net.net.xml',
    '-r', 'input_routes.rou.xml',
    '--step-length', str(TS),
    '--fcd-output', 'fcd.xml',
    '--no-step-log', ]
if not WATCH:
    cmd += ['-S', '-Q']

traci.start(cmd)

vehID = "ego"
vehInSim = False

while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    t = traci.simulation.getTime()
    if vehID in traci.simulation.getDepartedIDList():
        vehInSim = True
    if vehID in traci.simulation.getArrivedIDList():
        vehInSim = False
    if vehInSim:
        v = traci.vehicle.getSpeed(vehID)
        laneID = traci.vehicle.getLaneID(vehID)
        lanePos = traci.vehicle.getLanePosition(vehID)
        # if the speed is sufficiently high, we'll make it drive a bit slower
        # (retroactively)
        DELTA_V = 1
        if v > DELTA_V:
            v2 = v - DELTA_V
            lanePos2 = lanePos - TS * DELTA_V
            if lanePos2 < 0:
                print("not using full DELTA_V at time %s (pending #11081)" % t)
                lanePos2 = 0
                # we could also traverse the network graph to determine the
                # previous/next lane ourselves.
            print(v, v2)
            traci.vehicle.moveTo(vehID, laneID, lanePos2)
            traci.vehicle.setPreviousSpeed(vehID, v2)

traci.close()
