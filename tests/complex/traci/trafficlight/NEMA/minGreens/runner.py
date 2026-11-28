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
# @author  blessedefe4real
# @date    2026-11-07

import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "-a", "input_additional.add.xml",
             "--no-step-log",
             ] + sys.argv[1:])

tlsID = "2881"
greenStart = None
durations = []
for step in range(360):
    if step == 115:
        # Phase 4 is already green. Apply its new minimum at the cycle boundary.
        traci.trafficlight.setNemaMinGreens(tlsID, [0, 0, 0, 19, 0, 0, 0, 0])
    traci.simulationStep()
    time = traci.simulation.getTime()
    green = traci.trafficlight.getRedYellowGreenState(tlsID)[1] == "G"
    if green and greenStart is None:
        greenStart = time
    elif not green and greenStart is not None:
        durations.append(time - greenStart)
        greenStart = None

traci.close()
assert durations == [10, 10, 19, 19], durations
