#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
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
# @author  Mirko Barthauer
# @date    2022-10-26


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
             "-r", "input_routes.rou.xml",
             "--lanechange-output", "lanechanges.xml",
             "--lateral-resolution", "0.875",
             "--no-step-log"
             ] + sys.argv[1:])

vehID = "ego"
LANE_CHANGE_DURATION = 3

step = 0
while step < 100:
    step = traci.simulation.getTime()
    traci.simulationStep()
    if step == 3:
        traci.vehicle.changeLaneRelative(vehID, 1, LANE_CHANGE_DURATION)
        traci.vehicle.highlight(vehID, alphaMax=255, duration=0.1)
    if step == 10:
        traci.vehicle.changeLaneRelative(vehID, 1, LANE_CHANGE_DURATION)
        traci.vehicle.highlight(vehID, alphaMax=255, duration=0.1)

traci.close()
