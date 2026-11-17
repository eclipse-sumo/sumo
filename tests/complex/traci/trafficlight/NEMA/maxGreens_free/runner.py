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

# test setNemaMaxGreens in free mode (coordinate-mode=false), see #18513

from __future__ import print_function
from __future__ import absolute_import
import os
import sys

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa
import sumolib  # noqa

traci.start([sumolib.checkBinary('sumo'),
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "-a", "input_additional.add.xml,input_additional2.add.xml",
             "--no-step-log",
             ] + sys.argv[1:])

tlsID = "2881"
greenStart = None
for step in range(600):
    if step == 120:
        # phase 4 (the only one with state index 1 green): max green 21 -> 12 s
        traci.trafficlight.setNemaMaxGreens(tlsID, [0, 0, 0, 12, 0, 0, 0, 0])
        print("set max green of phase 4 to 12 at time", traci.simulation.getTime())
    traci.simulationStep()
    time = traci.simulation.getTime()
    green = traci.trafficlight.getRedYellowGreenState(tlsID)[1] == "G"
    if green and greenStart is None:
        greenStart = time
    elif not green and greenStart is not None:
        print("phase 4 green from %s for %s s" % (greenStart, time - greenStart))
        greenStart = None

traci.close()
