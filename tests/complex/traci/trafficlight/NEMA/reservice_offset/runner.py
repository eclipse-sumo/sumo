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
# @author  Justin Stewart
# @date    2026-11-06

# An offset change queued while the coordinated phases 2 and 6 are served again (reservicePhases)
# is applied at their next start, where they get their normal timing again.

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
             "-a", "input_additional.add.xml,input_additional2.add.xml,input_additional3.add.xml",
             "--no-step-log",
             ] + sys.argv[1:])

tlsID = "0"
sideStart = None
while traci.simulation.getTime() < 450:
    if traci.simulation.getTime() == 180:
        traci.trafficlight.setNemaOffset(tlsID, "10")
    traci.simulationStep()
    if sideStart is None and traci.simulation.getTime() > 200 and traci.trafficlight.getRedYellowGreenState(tlsID)[6] == "G":  # noqa
        sideStart = traci.simulation.getTime()
        print("phase 4 green at", sideStart)

traci.close()
