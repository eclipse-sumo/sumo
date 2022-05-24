#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
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
# @author  Matthias Schwamborn
# @date    2022-03-01

from __future__ import print_function
import os
import sys

SUMO_HOME = os.path.abspath(os.environ['SUMO_HOME'])
sys.path.append(os.path.join(SUMO_HOME, "tools"))

import traci  # noqa
import sumolib  # noqa
import traci.constants as tc  # noqa

sumoBinary = sumolib.checkBinary('sumo')
traci.start([sumoBinary,
             "-n", "input_net.net.xml",
             "-r", "input_routes.rou.xml",
             "--fcd-output", "fcd.xml",
             "--fcd-output.acceleration",
             "--no-step-log",
             ] + sys.argv[1:])

vehID = "v0"


while traci.simulation.getMinExpectedNumber() > 0:
    t = traci.simulation.getTime()
    if t == 5:
        traci.vehicle.setSpeedFactor(vehID, 0)
    if t == 20:
        traci.vehicle.setSpeedFactor(vehID, 2)
    traci.simulationStep()

traci.close()
